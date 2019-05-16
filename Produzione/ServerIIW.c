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

#define fflush(stdin) while(getchar() != '\n')
#define htdocsPath "htdocs"
 
pthread_mutex_t *fileMessagesAccess;
pthread_mutex_t *fileUsersAccess;
pthread_mutex_t *counter;

__thread int sock;
struct sigaction act;

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
    char comunicazioneServer[1024];
	int ricevuti;
	while(ricevuti = recv(sock , buffer, 4096, 0)> 0){
		
        // Ricevo il messaggio, ma devo comunque dare una risposta
        char out[2000];
        sprintf(out, "\nSocket numero: %d\n", sock, buffer);
        puts(out);
        int bytes_read;
        int fd;
		char *reqline[4], *requestType, *host, *userAgent, *accept, *requestedFile, *httpVersion;

		/* Ottengo tutte quante le informazioni dalla richiesta */
        reqline[0] = strtok(buffer, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0){ //perche' questo if? reqline[0] ha altre cose oltre Head o Get??
			requestType = reqline[0];
        } else if ( strncmp(reqline[0], "HEAD\0", 5) == 0){
			requestType = reqline[0];
		}

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
			reqline[3] = strtok(NULL, "\n");
		}

		char *fileType = malloc(5*sizeof(char));

        if ( strncmp(reqline[1], "/\0", 2)==0){
    	    requestedFile = "htdocs/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...
		} else {
            char *fileName= strtok(reqline[1], " ");
			requestedFile = malloc(strlen(fileName) + 7);
			memset(requestedFile, 0, strlen(fileName) + 7);
			strcat(requestedFile, htdocsPath);
			strcat(requestedFile, fileName);
			fileType = strrchr(fileName, '.');
			printf("Il tipo di file è: %s", fileType + 1);
		}

		httpVersion = reqline[2];

		printf("\nRequested file: %s\nRequest type is: %s\nHost is: %s\nAccept is: %s\nUser-Agent is: %s\nHTTP Version is: %s\n\n", requestedFile, requestType, host, accept, userAgent, httpVersion);

		struct stat buffer;   
		if (stat(requestedFile, &buffer) != 0){
			struct stat s;
			fd=open("404.html", O_RDONLY);
            fstat(fd, &s);
            char responseMessage[1000];
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", requestedFile);
            sprintf(responseMessage,"HTTP/1.1 404 Not Found\nContent-Length: %d\nLast-Modified: %s\n\n", strlen(data_to_send), ctime(&s.st_mtime));
            send(sock, responseMessage, strlen(responseMessage), 0);
			write (sock, data_to_send, strlen(data_to_send));
			close(fd);

		} else {

			if (strcmp(fileType, ".jpg") == 0){
				printf("\nE' un'immagine!\n");
				int *size = malloc(sizeof(int));
				hashNode *testAddress;
				testAddress=NULL;
				while(testAddress==NULL){
					testAddress = searchHashNode(requestedFile, 1920, 1080, 10, 10, size);
			}       

			char responseMessage[1000];
			sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\n\n", testAddress->imageSize);
			send(sock, responseMessage, strlen(responseMessage), 0);       
			write(sock, testAddress->imageBuffer, testAddress->imageSize);
			pthread_rwlock_unlock(&(testAddress->sem));
			
			} else {

				fd=open(requestedFile, O_RDONLY);
				printf("\nValue of fd is: %d\n", fd);
				struct stat s;
				fstat(fd, &s);
				char responseMessage[1000];
				sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\nLast-Modified: %s\n\n", s.st_size, ctime(&s.st_mtime));
				send(sock, responseMessage, strlen(responseMessage), 0);            
				if(strcmp(requestType, "GET") == 0){
					while ((bytes_read=read(fd, data_to_send, 4096))>0)
						write(sock, data_to_send, 4096);
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
	
	fileMessagesAccess = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(fileMessagesAccess, NULL);
	fileUsersAccess = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(fileUsersAccess, NULL);
	counter = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(counter, NULL);

	char buff[20];
    struct tm *sTm;
    int socket_desc , *socket_cliente , c , read_size;
	int utenti_connessi = 0;
    struct sockaddr_in server , client;
    char client_message[2000];
    pthread_t thread, thread1, thread2;


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
     
    puts("\nIn attesa di connessione...");
    c = sizeof(struct sockaddr_in);
     
    while(1){
	time_t ltime; 
    ltime=time(NULL); 
		
	socket_cliente = malloc(sizeof(int));
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