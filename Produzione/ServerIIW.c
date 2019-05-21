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

#define fflush(stdin) while(getchar() != '\n')
#define htdocsPath "htdocs" 

__thread int sock;
struct sigaction act;

ssize_t writen(int fd, const void *buf, size_t n){
 size_t nleft;
 ssize_t nwritten;
 const char *ptr;
 ptr = buf;  
 nleft = n;
 while (nleft > 0) {
   if ( (nwritten = send(fd, ptr, nleft,0)) < 0) {
      if ((nwritten < 0) && (errno == EINTR))
	   nwritten = 0;
      else
	  pthread_exit(-1); /* errore */
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
   if ( (nread = read(fd, ptr, nleft)) < 0) {
       if (errno == EINTR)/* funzione interrotta da un segnale prima di averpotuto leggere qualsiasi dato. */ 
         nread = 0;  
       else
	   	 pthread_exit(-1); /*errore */
	}
   nleft -= nread; 
   ptr += nread; 
 return(nleft);/* return >= 0 */
}


void *gestore_utente(void *socket){
    char data_to_send[4096];
    char buff[2000];
    struct tm *sTm;
    int scelta, valore_ritorno, uscita_thread, read_size;
	sock = *((int*)socket);

	// Struttura dati per impostare il timeout della connessione
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	
    char *prova = malloc(1024*sizeof(char));
	char *buffer = malloc(4096);
	if(!prova){
		perror("Malloc fallita\n");
		return -1;
	}
	if(!buffer){
		perror("Malloc fallita\n");
		return -1;
	}
    char comunicazioneServer[1024];
	int ricevuti;
	while((ricevuti = readn(sock , buffer, 4096)) > 0){
             if(ricevuti<20){
                perror("Messaggio ricevuto troppo piccolo\n");
                close(sock);
                pthread_exit(-1);
             }

		printf("\n%s\n", buffer);
		
        // Ricevo il messaggio, ma devo comunque dare una risposta
        char out[2000];
        sprintf(out, "\nSocket numero: %d\n%s\n", sock, buffer);
        int bytes_read;
        int fd;
		char *reqline[4], *requestType, *host, *userAgent, *accept, *requestedFile, *httpVersion;

		/* Ottengo tutte quante le informazioni dalla richiesta */
        reqline[0] = strtok(buffer, " \t\n");
		requestType = reqline[0];
        
		reqline[1] = strtok(NULL, " \t");
		reqline[2] = strtok(NULL, " \t\n");
		reqline[3] = strtok(NULL, "\n");
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

		char *fileType = malloc(5*sizeof(char));

        if ( strncmp(reqline[1], "/\0", 2)==0){
				requestedFile = "htdocs/index.html";
				fileType = "html";

		} else {
				char *fileName= strtok(reqline[1], " ");
				int lun=strlen(fileName);
				char *fileName2 = malloc(sizeof(char)*strlen(fileName));
				strcpy(fileName2, fileName);
				fileName2 = strtok(fileName2, ".");
				if (strlen(fileName2) != lun){
					requestedFile = malloc(strlen(fileName) + 7);
					memset(requestedFile, 0, strlen(fileName) + 7);
					strcat(requestedFile, htdocsPath);
					strcat(requestedFile, fileName);
					fileType = strrchr(fileName, '.') + 1;
				} else{
					requestedFile = malloc(strlen(fileName) + 18);
					memset(requestedFile, 0, strlen(fileName) + 18);
					strcat(requestedFile, htdocsPath);
					strcat(requestedFile, fileName);
					if (requestedFile[strlen(requestedFile)-1] == '/'){
						strcat(requestedFile, "index.html");
					} else {
						strcat(requestedFile, "/index.html");
					}
					fileType = "html";
				} 
		}
	
		httpVersion = reqline[2];
		struct stat buffer;   
		if (stat(requestedFile, &buffer) != 0){
			struct stat s;
			fd=open("htdocs/404.html", O_RDONLY);
			if(fd==-1){
					perror("Apertura file fallita\n");
					pthread_exit(-2);
			}

            fstat(fd, &s);
            char responseMessage[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
            sprintf(responseMessage,"HTTP/1.1 404 Not Found\nContent-Length: %d\nLast-Modified: %s\n\n", strlen(data_to_send), ctime(&s.st_mtime));
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
					perror("Scrittura su socket fallita\n");
					pthread_exit(-1);
			}
			if(writen(sock, data_to_send, strlen(data_to_send))<0){
					perror("Scrittura su socket fallita\n");
					pthread_exit(-1);
			}
			close(fd);
		} else {

			if (strcmp(fileType, "jpg") == 0){
				int *size = malloc(sizeof(int));
				int width, height;

				runDetection(userAgent, &width, &height);
				
				hashNode *testAddress;
				testAddress=NULL;
				while(testAddress==NULL){
					testAddress = searchHashNode(requestedFile, width, height, 0, size, fileType);
			}

			char responseMessage[1000];
			sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\n\n", testAddress->imageSize);
			
			if(writen(sock, responseMessage, strlen(responseMessage))<0){
				perror("Scrittura su socket fallita\n");
				pthread_exit(-1);
			}
			if(writen(sock, testAddress->imageBuffer, testAddress->imageSize)<0){
 				perror("Scrittura su socket fallita\n");
				pthread_exit(-1);
  			}

			pthread_rwlock_unlock(testAddress->sem);
			
			} else {
				fd=open(requestedFile, O_RDONLY);
				if(fd<0){
					perror("Apertura file fallita\n");
					pthread_exit(-2);
				}
				struct stat s;
				fstat(fd, &s);
				char responseMessage[1000];
				sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\nLast-Modified: %s\n\n", s.st_size, ctime(&s.st_mtime));
				
				if(writen(sock, responseMessage, strlen(responseMessage))<0){
					perror("Scrittura su socket fallita\n");
					pthread_exit(-1);
				}            
				if(strcmp(requestType, "GET") == 0){

				while ((bytes_read=read(fd, data_to_send, 1024))>0) {
					if(writen(sock, data_to_send, 1024)<0){
							perror("Scrittura su socket fallita\n");
							pthread_exit(-1);
							}
						}
				}
				close(fd);
		}
    }
}
	
	time_t now = time (0);
	sTm = gmtime (&now);
	strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);

	sprintf(comunicazioneServer, "%s | Socket numero: %d | Connessione interrotta", buff, sock);
	puts(comunicazioneServer);
	close(sock);
	free(socket);
	pthread_exit((int*)-1);
}	

int main(int argc , char *argv[]){			
	
	int porta;

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
    pthread_t thread;

	startDetectionProvider(10,  1000);

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        perror("Errore nella creazione del Socket.");
		return -1;
    }
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(porta);
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Bind non riuscita.");
        return 1;
    }
     
    listen(socket_desc , 3);
    //puo' fallire??

    puts("\nIn attesa di connessione...");
    c = sizeof(struct sockaddr_in);
     
    while(1){
	time_t ltime; 
    ltime=time(NULL); 
		
	socket_cliente = malloc(sizeof(int));
    if(!socket_cliente){
		perror("Malloc fallita\n");
		return -1;
	}
	*socket_cliente = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    
		if (*socket_cliente < 0)
		{
			perror("Connessione rifiutata.");
			return 1;
		}

		time_t now = time (0);
		sTm = gmtime (&now);
		strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
		char comunicazioneConnessione[1024];
		sprintf(comunicazioneConnessione, "\n%s | Socket numero: %d | Nuova connessione accettata", buff, *socket_cliente);
		puts(comunicazioneConnessione);
		pthread_create(&thread, NULL, gestore_utente, (void*)socket_cliente);
    }
    return 0;
}