#include <stdio.h>
#include <string.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "newHash.h"
#include "detection.h"

#define htdocsPath "htdocs" 

FILE *logFile;
__thread int sock;
__thread char buff[2000];
__thread struct tm *sTm;

struct sigaction act;

ssize_t writen(int fd, const void *buf, size_t n){//send
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
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
			pthread_exit((int*)-1);	
		}
	} 
	nleft -= nwritten;
	ptr += nwritten;
	}
	//printf("\nFinito di trasmettere? %d", nleft);
	return(nleft);
}

int readn(int fd, void *buf, size_t n) { //recv
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
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
			//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
			pthread_exit((int*)-1);	
		}
	}
	nleft -= nread; 
	return(nleft);/* return >= 0 */
}

void *file_flush(){
	while(1){
		sleep(30);
		fflush(logFile);
	}
}

void *gestore_utente(void *socket){
    char data_to_send[4096];
    int scelta, valore_ritorno, uscita_thread, read_size;
	sock = *((int*)socket);

	// Struttura dati per impostare il timeout della connessione
    struct timeval tv;
    tv.tv_sec = 6;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	
	char *buffer = calloc(1, 4096*sizeof(char));//malloc+memset

	if(!buffer){
		 	time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
			close(sock);
		pthread_exit(-1);
	}
    char comunicazioneServer[1024];
	int ricevuti;
	while((ricevuti = readn(sock , buffer, 4096)) > 0){
		if(ricevuti>4076){
			time_t now = time (0);
			sTm = gmtime (&now);
			strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
			close(sock);
			free(buffer);
			free(socket);
			pthread_exit((int*)-1);	
		}

		// printf("\n%s\n", buffer);
		
        // Ricevo il messaggio, ma devo comunque dare una risposta
        int bytes_read;
        int fd;
		char *reqline[4], *requestType, *host, *userAgent, *accept, *requestedFile, *httpVersion;

		/* Ottengo tutte quante le informazioni dalla richiesta */
        reqline[0] = strtok(buffer, " \t\n");
		if (reqline[0] == NULL){
			char responseMessage[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Bad Request</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
			sprintf(responseMessage,"HTTP/1.1 400 Bad Request\nContent-Length: %d\n\n", strlen(data_to_send));
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}
			if(writen(sock, data_to_send, strlen(data_to_send))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}
		}

		requestType = reqline[0];
		if ( (reqline[1] = strtok(NULL, " \t") ) == NULL){
			char responseMessage[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Bad Request</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
			sprintf(responseMessage,"HTTP/1.1 400 Bad Request\nContent-Length: %d\n\n", strlen(data_to_send));
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(socket);
				pthread_exit((int*)-1);	
			}
			if(writen(sock, data_to_send, strlen(data_to_send))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}
			close(sock);
			free(buffer);
			free(socket);
			pthread_exit((int*)-1);	
		}

		if ( (reqline[2] = strtok(NULL, " \t\n") ) == NULL){
			char responseMessage[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Bad Request</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
			sprintf(responseMessage,"HTTP/1.1 400 Bad Request\nContent-Length: %d\n\n", strlen(data_to_send));
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(socket);
				pthread_exit((int*)-1);	
			}
			if(writen(sock, data_to_send, strlen(data_to_send))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}
			close(sock);
			free(buffer);
			free(socket);
			pthread_exit((int*)-1);	
		}
		
		if ( (reqline[3] = strtok(NULL, "\n") ) == NULL){
			char responseMessage[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Bad Request</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
			sprintf(responseMessage,"HTTP/1.1 400 Bad Request\nContent-Length: %d\n\n", strlen(data_to_send));
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}
			if(writen(sock, data_to_send, strlen(data_to_send))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}
			close(sock);
			free(buffer);
			free(socket);
			pthread_exit((int*)-1);	
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

		char* fileType = calloc(1, 10*sizeof(char));
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
			fd=open("htdocs/404.html", O_RDONLY);
			if(fd==-1){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Apertura del file fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);		
			}

            fstat(fd, &s);
			char headerSend[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
            sprintf(headerSend,"HTTP/1.1 404 Not Found\nContent-Length: %d\nLast-Modified: %s\n\n", strlen(data_to_send), ctime(&s.st_mtime));

			if(writen(sock, headerSend, strlen(headerSend))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}

			if(writen(sock, data_to_send, strlen(data_to_send))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}

			close(fd);
			continue;
		} else {

			if (strcmp(fileType, "jpg") == 0){
				int width, height, size;

				//runDetection(userAgent, &width, &height);
				
				hashNode *testAddress;
				testAddress=NULL;
				while(testAddress==NULL){
					testAddress = searchHashNode(requestedFile, 0, 0, 0, &size, fileType);
			}

			char responseMessage[1000];
			sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\n\n", testAddress->imageSize);
			
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}

			if(writen(sock, testAddress->imageBuffer, testAddress->imageSize)<0){
				time_t now = time (0);
				sTm = gmtime (&now);
				strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
				fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
				//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
				close(sock);
				free(buffer);
				free(socket);
				pthread_exit((int*)-1);	
			}

			pthread_rwlock_unlock(testAddress->sem);
			continue;
			
			} else {

				fd=open(requestedFile, O_RDONLY);
				if(fd<0){
					//fprintf(stderr, "%s", "Apertura file fallita\n");
					time_t now = time (0);
					sTm = gmtime (&now);
					strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
					fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
					close(sock);
					free(buffer);
					free(socket);
					pthread_exit((int*)-1);	
				}
				struct stat s;
				fstat(fd, &s);
				char responseMessage[1000];
				sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\nLast-Modified: %s\n\n", s.st_size, ctime(&s.st_mtime));
				
				if(writen(sock, responseMessage, strlen(responseMessage))<0){
					time_t now = time (0);
					sTm = gmtime (&now);
					strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
					fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
					//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
					close(sock);
					free(buffer);
					free(socket);
					pthread_exit((int*)-1);	
				}

				if(strcmp(requestType, "GET") == 0){
					while ( (bytes_read=read(fd, data_to_send, 1024)) >0) {
						//printf("Bytes read are: %d", bytes_read);
						if(writen(sock, data_to_send, 1024) <0 ){
							time_t now = time (0);
							sTm = gmtime (&now);
							strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
							fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
							//fprintf(stderr, "%s", "Scrittura su socket fallita\n");
							close(sock);
							free(buffer);
							free(socket);
							pthread_exit((int*)-1);	
						}
					}
				}
			}
			close(fd);
			continue;
   	 	}
	}

	time_t now = time (0);
	sTm = gmtime (&now);
	strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	fprintf(logFile, "%s | Socket numero: %d | Connessione interrotta\n", buff, sock);
	close(sock);
	free(buffer);
	free(socket);
	pthread_exit((int*)-1);	
}	

int main(int argc , char *argv[]){			
	
	int porta;

	logFile = fopen ("server.log", "a");

	if (argc > 2 || argc == 1){

			printf("\nUtilizzo: Server [opzioni]\n\nOpzioni:\n  indirizzo                    Avvia il server utilizzando la porta indicata \n  --seeuser                    Mostra la lista degli utenti registrati\n");
			return 0;
		}
	
	if (argc == 2){
		if (strcmp(argv[1], "--seeuser") == 0){
			return 0;
	} 
	
	else if(strcmp(argv[1], "--help") == 0){
		return 0;
	} else {
		porta = atoi(argv[1]);
	}
	} 
	
	
	char buff[20];
    struct tm *sTm;
    int socket_desc , *socket_cliente , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
    pthread_t thread, fileFlush;;

	//startDetectionProvider(10,  1000);

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
		//fprintf(stderr, "%s", "Errore nella creazione della socket\n");
		return -1;
    }
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(porta);
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
		fprintf(stderr, "%s", "Bind non riuscita\n");
        return 1;
    }
     
    listen(socket_desc , 3);
    //puo' fallire??

    puts("\nIn attesa di connessione...");
    c = sizeof(struct sockaddr_in);
	pthread_create(&fileFlush, NULL, file_flush, NULL);

    while(1){
		time_t ltime; 
		ltime=time(NULL); 
			
		socket_cliente = malloc(sizeof(int));
		if(!socket_cliente){
			fprintf(stderr, "%s", "Malloc fallita\n");
			return -1;
			}
		*socket_cliente = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		
		if (*socket_cliente < 0){
			fprintf(stderr, "%s", "Connessione rifiutata\n");
			return 1;
		}

		time_t now = time (0);
		sTm = gmtime (&now);
		strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
 		char comunicazioneConnessione[1024];
		fprintf(logFile, "\n%s | Socket numero: %d | Nuova connessione accettata\n", buff, *socket_cliente);
		pthread_create(&thread, NULL, gestore_utente, (void*)socket_cliente);
    }
    return 0;
}