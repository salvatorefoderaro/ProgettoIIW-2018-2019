#include <string.h>    
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "newHash.h"
#include "detection.h"
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "config.h"
#define _GNU_SOURCE

FILE *logFile;
__thread int sock;
__thread char buff[800];
__thread struct tm *sTm;
__thread char *buffer;
__thread char responseMessage[1000];
__thread int *socketSender;
__thread char data_to_send[4096];
struct sigaction act;

ssize_t writen(int fd, const void *buf, size_t n){
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	ptr = buf;  
	nleft = n;
	while (nleft > 0) {
	if ( (nwritten = send(fd, ptr, nleft,MSG_NOSIGNAL)) < 0) {
		if ((nwritten < 0) && (errno == EINTR))
		nwritten = 0;
		else{
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, fd);
			close(fd);
			free(buffer);
			free(socketSender);
			pthread_exit((void*)-1);
		}
	} 
	nleft -= nwritten;
	ptr += nwritten;
	}
	return(nleft);
}

int readn(int fd, void *buf, size_t n) { 
 	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr = buf;
	nleft = n; 
	if ( (nread = recv(fd, ptr, nleft, MSG_NOSIGNAL)) < 0) {
		if (errno == EINTR)/* funzione interrotta da un segnale prima di averpotuto leggere qualsiasi dato. */ 
			nread = 0;
		else{
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, fd);
			close(fd);
			free(buffer);
			free(socketSender);
			pthread_exit((void*)-1);	
		}
	}
	nleft -= nread; 
	return(nleft);/* return >= 0 */
}

void sigHandler(int signal){
	printf("\nEffettuo il flush del file...\n");
	time_t now = time (0);
	sTm = gmtime (&now);
	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	fprintf(logFile, "%s | Chiusura del server\n", buff);
	fflush(logFile);
	fclose(logFile);
	exit(-1);
}

void *file_flush(){
	while(1){
		sleep(flushTime);
		fflush(logFile);
	}
}

void sendBadRequest(int socket){
	sprintf(responseMessage,"HTTP/1.1 400 Bad Request\n\n");
	writen(socket, responseMessage, strlen(responseMessage));
}

void sendNotFound(int socket, char *requestedFile){
	sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
	sprintf(responseMessage,"HTTP/1.1 404 Not Found\nContent-Length: %ld\n\n", strlen(data_to_send));
	writen(socket, responseMessage, strlen(responseMessage)); 
	writen(socket, data_to_send, strlen(data_to_send));
} 

void sendFoundHeader(int socket, int size, char* type){
	if(strcmp(type, "html") == 0){
		char *type = "text/html";
	} else if(strcmp(type, "image") == 0){
		char *type = "image";
	} else {
		char *type = "undefined";
	}

	sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\ncontent-type: %s\n\n", size, type);
	writen(sock, responseMessage, strlen(responseMessage));
}

void *requestHandler(void *socket){

	socketSender = socket;
	sock = *((int*)socket);

	// Struttura dati per impostare il timeout della connessione
    struct timeval tv;
    tv.tv_sec = socketTimeout;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
	
	buffer = calloc(1, 4096*sizeof(char));//malloc+memset

	if(!buffer){
		time_t now = time (0);
		sTm = gmtime (&now);
		strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
		fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
		close(sock);
		free(buffer);
		free(socket);
		pthread_exit((void*)-1);
	}

	int ricevuti;
	while( (ricevuti = readn(sock , buffer, 4096)) ){
		if(ricevuti>4076){
			sendBadRequest(sock);
		}

        int bytes_read, quality = 0, fd;
		char *reqline[4], *requestType, *host, *userAgent, *accept, *requestedFile, *httpVersion;

		/* Ottengo tutte quante le informazioni dalla richiesta */
        reqline[0] = strtok(buffer, " \t\n");
		if (reqline[0] == NULL){
			sendBadRequest(sock);
			continue;
		}

		requestType = reqline[0];
		if ( (reqline[1] = strtok(NULL, " \t") ) == NULL){
			sendBadRequest(sock);
			continue;
		}

		if ( (reqline[2] = strtok(NULL, " \t\n") ) == NULL){
			sendBadRequest(sock);
			continue;
		}
		
		if ( (reqline[3] = strtok(NULL, "\n") ) == NULL){
			sendBadRequest(sock);
			continue;
		}

		while (reqline[3] != NULL){
			if(strncmp(reqline[3], "Host:", 5) == 0){
				host = reqline[3];
			} else if(strncmp(reqline[3], "User-Agent:", 11) == 0){
				userAgent= reqline[3];
			} else if(strncmp(reqline[3], "Accept:", 7) == 0){
				accept = reqline[3];
			}
			reqline[3] = strtok(NULL,"\n");
		}

		if (accept != NULL){
			accept = strtok(accept, ":");
			accept = strtok(NULL, "\0");
			char *otherString = accept;
			do {
				char *string = strtok(otherString,";");
				char *qValue = strtok(NULL, ",");
				otherString = strtok(NULL, ",");
				if (strstr(string, "image") != NULL && strstr(string, "q=") != NULL){
					qValue = strtok(qValue, "=");
					qValue = strtok(NULL, ".");
					qValue = strtok(NULL, "\0");
					quality =  atoi(qValue)*10;
					break;
				}
			} while(otherString != NULL);
		}

		char* fileType = calloc(1, 10*sizeof(char));
		if(!fileType){
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
			close(sock);
			free(buffer);
			free(socket);
			pthread_exit((void*)-1);
		}
        if ( strncmp(reqline[1], "/\0", 2) ==0 ){
				requestedFile = "htdocs/index.html";
				strncpy(fileType, "html", 10*sizeof(char));
		} else {
				char *fileName= strtok(reqline[1], " ");
				int lun=strlen(fileName);
				char *fileName2 = calloc(1, sizeof(char)*strlen(fileName));
				strncpy(fileName2, fileName, strlen(fileName) );
				fileName2 = strtok(fileName2, ".");

				if (strlen(fileName2) != lun){
					requestedFile = calloc(1, strlen(fileName) + 7);
					memset(requestedFile, 0, strlen(fileName) + 7);
					strcat(requestedFile, htdocsPath);
					strcat(requestedFile, fileName);
					fileType = strrchr(fileName, '.') + 1;

				} else{

					requestedFile = calloc(1, strlen(fileName) + 18);
					memset(requestedFile, 0, strlen(fileName) + 18);
					strcat(requestedFile, htdocsPath);
					strcat(requestedFile, fileName);
					if (requestedFile[strlen(requestedFile)-1] == '/'){
						strcat(requestedFile, "index.html");
					} else {
						strcat(requestedFile, "/index.html");
					}
					strncpy(fileType, "html", 10*sizeof(char));
				}
			free(fileName2);
		}
	
		httpVersion = reqline[2];

		struct stat s;

		if (stat(requestedFile, &s) != 0){
			sendNotFound(sock, requestedFile);
			continue;
		} else {
			if ( strcmp(fileType, "jpg") == 0 || strcmp(fileType, "jpeg") == 0 || strcmp(fileType, "png") == 0){
				int width, height, size;

				#ifdef detection       
					runDetection(userAgent, &width, &height);
				#else
					width = 0;
					height = 0;
				#endif
				
				hashNode *testAddress;
				testAddress=NULL;
				while(testAddress==NULL){
					testAddress = searchHashNode(requestedFile, width, height, quality, &size, fileType);
			}

			sendFoundHeader(sock, testAddress->imageSize, "image");
			writen(sock, testAddress->imageBuffer, testAddress->imageSize);
			pthread_rwlock_unlock(testAddress->sem);
			continue;
			
			} else {

				fd=open(requestedFile, O_RDONLY);
				if(fd<0){
					time_t now = time (0);
					sTm = gmtime (&now);
					strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
					fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
					close(sock);
					free(buffer);
					free(socket);
					close(fd);
					pthread_exit((void*)-1);	
				}
				struct stat s;
				fstat(fd, &s);

				sendFoundHeader(sock, s.st_size, fileType);

				if(strcmp(requestType, "GET") == 0){
					while ((bytes_read=read(fd, data_to_send, 1024))>0) {
						writen(sock, data_to_send, 1024);
					}
				}
			}
			close(fd);
			continue;
   	 	}
	}
}	

int main(int argc , char *argv[]){			
	
	int porta;

	logFile = fopen ("server.log", "a");
	if(!logFile){
		perror("\nErrore nella funzione fopen()!\n");
		exit(-1);
	}

	if (argc > 3 || argc == 1){
		printf("\nUtilizzo: Server [opzioni]\n\nOpzioni:\n  indirizzo                    Avvia il server utilizzando la porta indicata\n");
		return 0;
	}
	
	porta = atoi(argv[1]);
	
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigHandler;
    sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);
    sigaction(SIGKILL, &action, NULL);

	time_t now = time (0);
	sTm = gmtime (&now);
	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	fprintf(logFile, "%s | Avvio del server\n", buff);
	
	char buff[20];
    struct tm *sTm;
    int socket_desc , *socket_cliente, reuse = 1;
    struct sockaddr_in server;
    pthread_t *thread, *fileFlush;

	fileFlush = malloc(sizeof(pthread_t));
	if (!fileFlush){
		perror("\nErrore nella funzione malloc()!\n");
		exit(-1);
	}


	#ifdef detection
		startDetectionProvider(workSetDetection,  cacheItemDetection);
	#endif

	if (pthread_create(fileFlush, NULL, file_flush, NULL) != 0){
		perror("\nErrore nella funzione pthread_create()!\n");
		exit(-1);
	}

	if ((socket_desc = socket(AF_INET , SOCK_STREAM , 0)) < 0){
		perror("\nErrore nella funzione socket()!\n");
		exit(-1);
	}

	if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
		perror("\nErrore nella funzione setsockopt()!\n");
		exit(1);
	}

	memset((void *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(porta);
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
		perror("\nErrore nella funzione bind()!\n");
        exit(-1);
    }
     
    if (listen(socket_desc , backlog) < 0){
		perror("\nErrore nella funzione listen()!\n");
		exit(-1);
	}

    puts("\nIn attesa di connessione...");

    for(;;){			

		socket_cliente = calloc(1, sizeof(int));
		if(!socket_cliente){
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Errore nella malloc relativo al socket\n", buff);
			close(sock);
			free(buffer);
			free(socket_cliente);
			continue;
		}
		
		*socket_cliente = accept(socket_desc, (struct sockaddr *)NULL, NULL);
		if (*socket_cliente < 0){
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Connessione rifiutata\n", buff);
			close(sock);
			free(buffer);
			free(socket_cliente);
			continue;
		}

		time_t now = time (0);
		sTm = gmtime (&now);
		strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
		fprintf(logFile, "\n%s | Socket numero: %d | Nuova connessione accettata\n", buff, *socket_cliente);
		thread = malloc(sizeof(pthread_t));

		if (!thread){
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Errore nella malloc del thread\n", buff);
			close(sock);
			free(buffer);
			free(socket_cliente);
			continue;			
		}
		
		if (pthread_create(thread, NULL, requestHandler, (void*)socket_cliente) != 0){
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Errore nella creazione del thread\n", buff);
			close(sock);
			free(buffer);
			free(socket_cliente);
			continue;
		}
    }
    return 0;
}