#include<stdio.h>
#include<string.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include <time.h>
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
#define fflush(stdin) while(getchar() != '\n')
#define file_utenti "user.dat"
#define file_messaggi "messages.dat"
 
/*	Codici errore (Corrispondono al valore di ritorno):
 * 
 *  -1 | Accesso già effettuato
 *	-2 | Effettua prima log-in
 *	-3 | Nome utente e Password non trovati
 *	-5 | Errore nell'apertura del file
 *  -6 | Nessun messaggio presente
 * 	-7 | Errore nella scrittura del file
 * 	-8 | Il messaggio non è presente nel sistema
 * 	-9 | Non hai il permesso per eliminare il messaggio!
 *  -10 | Nome utente già presente!
 *  -11 | Stringa vuota non permessa
 * 
 *  Codice operazione:
 *	 0 | Effettua accesso -> Argomento1: nome_utente | Argomento2: password
 *   1 | Leggi tutti i messaggi -> Argomento1: NULL | Argomento2: NULL
 * 	 2 | Inserisci messaggio -> Argomento1: oggetto_messaggio | Argomento2: testo_messaggio
 * 	 3 | Rimuovi messaggio -> Argomento1: id_messaggio | Argomento2: NULL
 *   4 | Effettua registrazione -> Argomento1: nome_utente | Argomento2: password
 *   5 | Disconnessione ->	Argomento1: NULL | Argomento2: NULL
 */ 
 
pthread_mutex_t *fileMessagesAccess;
pthread_mutex_t *fileUsersAccess;
pthread_mutex_t *counter;
 
struct comunicazione {
	int operazione;
	int valore_ritorno;
	char argomento1[512];
	char argomento2[512];
	};
 
struct consistenzaSessione{
	int lastMessage;
	int lastUser;
	int utenti_connessi;
};

struct sessione {
	int id_utente_loggato;
	char nome_utente_loggato[64];
	};

struct messaggi {
	int id_utente;
	int id_messaggio;
	char messaggio[512];
	char oggetto[128];
	char mittente[64];
};

struct utente {
	int id_utente;

	char nome_utente[64];
	char password_utente[64];
	};

struct consistenzaSessione session;
__thread struct sessione utente_loggato;
__thread char comunicazioneServer[1024];
__thread int sock;
__thread struct comunicazione ricezione;
struct sigaction act;

void decodeCommunication(char *buffer, struct comunicazione *struttura){
	/*
	Decodifica la stringa buffer, inserendo il risultato della tokenizzazione
	nei vari campi della struttura
	*/
	struttura->operazione = ntohs(atoi(strtok(buffer, "|")));
	struttura->valore_ritorno = ntohs(atoi(strtok(NULL, "|")));
	strcpy(struttura->argomento1, strtok(NULL, "|"));
	strcpy(struttura->argomento2, strtok(NULL, "|"));
}

char *encodeMessage(struct messaggi *struttura){
    /* 
	Codifica i campi della struttura del tipo messaggio,
	restituendo una stringa
	*/
    char *buffer = malloc(sizeof(struct messaggi)+4*sizeof(char));
    sprintf(buffer, "%d|%d|%s|%s|%s", htons(struttura->id_messaggio), htons(struttura->id_utente),struttura->messaggio, struttura->mittente, struttura->oggetto);
    return buffer;  
}      

int sendThroughSocket(int socket, int size, void *buffer){
	// Invia, utilizzando il socket indicato, un numero di byte pari a size presenti in buffer
	int nBytes = send(socket, buffer, size, 0);
	if (nBytes < 1){
		exit((int)-1);
	} else if (nBytes != size){
		exit((int)-1);
	}
	return nBytes;
}

int receiveThroughSocket(int socket, int size, void *buffer){
	// Riceve, utilizzando il socket indicato, un numero di byte pari a size scrivendoli in buffer
	int nBytes;
	nBytes = recv(socket, buffer , size , 0);
	if (nBytes < 1){
		exit((int)-1);
	} else if (nBytes != size){
		exit((int)-1);
	}
	return nBytes;
}

void *recupero_consistenza_file(){
	// Consistenza file messaggi
	int resto;
	FILE *infile;
    
    infile = fopen (file_messaggi, "r");
    if (infile == NULL)
    {
		pthread_exit((int*)-5);
    }
    	fseek(infile, 0, SEEK_END);
	resto = ftell(infile) % sizeof(struct messaggi);
	if (resto != 0){
		truncate(file_messaggi, ftell(infile) - resto);
		}
     fclose(infile);
     
     // Consistenza file utenti
    
    infile = fopen (file_utenti, "r");
    if (infile == NULL)
    {
		session.lastMessage = 0;
		pthread_exit((int*)-5);
    }
    	fseek(infile, 0, SEEK_END);
	resto = ftell(infile) % sizeof(struct utente);
	if (resto != 0){
		truncate(file_utenti, ftell(infile) - resto);
		}
     fclose(infile);
	

}
	
void *recupero_consistenza_sessione(){
	
	// Recupero consistenza messaggi
	
	FILE *infile;
    struct messaggi recuperoMessaggi;
    
    infile = fopen (file_messaggi, "r");
    if (infile == NULL)
    {
		session.lastMessage = 0;
		pthread_exit((int*)-5);
    }
    fseek(infile, 0, SEEK_END);
    if (ftell(infile) == 0){
		fclose(infile);
		session.lastMessage = 0;
		pthread_exit((int*)-5); // Nessun messaggio presente
    }
    fseek(infile, 0, SEEK_SET);

    fseek(infile, -(sizeof(struct messaggi)), SEEK_END);
    fread(&recuperoMessaggi, sizeof(struct messaggi), 1, infile);
    session.lastMessage = recuperoMessaggi.id_messaggio;
    fclose(infile);
    
    // Recuperto consistenza utenti
	
    struct utente recuperoUtente;
    
    infile = fopen (file_utenti, "r");
    if (infile == NULL)
    {
		session.lastUser = 0;
		pthread_exit((int*)-5);
    }
    fseek(infile, 0, SEEK_END);
    if (ftell(infile) == 0){
		fclose(infile);
		session.lastUser = 0;
		pthread_exit((int*)-5); // Nessun utente presente
    }
    fseek(infile, 0, SEEK_SET);
    fseek(infile, -(sizeof(struct utente)), SEEK_END);
    fread(&recuperoUtente, sizeof(struct utente), 1, infile);
    session.lastUser = recuperoUtente.id_utente;
    fclose(infile);
	
    return (int *)1;
	}

int elimina_messaggio(int id_utente, int id_messaggio){
	
    FILE *leggo;
    FILE *scrivo;
      
    pthread_mutex_lock(fileMessagesAccess);
    
    struct messaggi controllo;
    leggo = fopen (file_messaggi, "r");
    if (leggo == NULL)
    {
		pthread_mutex_unlock(fileMessagesAccess);
	
        return -5+1000; // Errore nell'apertura del file
    }
    
	scrivo = fopen ("passaggio.dat", "w+");
    if (scrivo == NULL)
    {
		pthread_mutex_unlock(fileMessagesAccess);
		fclose(leggo);
				 

        return -5+1000; // Errore nell'apertura del file
    }
    
    fseek(leggo, 0, SEEK_END);
    if (ftell(leggo) == 0){
		pthread_mutex_unlock(fileMessagesAccess);
		fclose(leggo);
		fclose(scrivo);

		return(-6+1000); // Nessun messaggio presente
    }
    
    fseek(leggo, 0, SEEK_SET);
    int occorrenze_messaggio = 0;
    int occorrenze_utente = 0;
    
	// Leggo tutto il file dei messaggi
    while(fread(&controllo, sizeof(struct messaggi), 1, leggo)){
                if(controllo.id_utente == id_utente && controllo.id_messaggio == id_messaggio){
						occorrenze_utente = 1;
                        occorrenze_messaggio = 1;
                        if (controllo.id_messaggio == session.lastMessage){
							pthread_mutex_lock(counter);
                            session.lastMessage = session.lastMessage - 1; // Se messaggio elimino è ultimo, decremento per la consistenza della sessione
							pthread_mutex_unlock(counter);
                        }
			continue;
		}
                if(controllo.id_messaggio == id_messaggio){
			occorrenze_messaggio = 1;
		}
                
            if(fwrite(&controllo, sizeof(struct messaggi), 1, scrivo) == 0){
			pthread_mutex_unlock(fileMessagesAccess);
			fclose(leggo);
			fclose(scrivo);

			return -7+1000; // Errore nella scrittura su file
		}
	}
        
    fclose(leggo);
    unlink(file_messaggi);
    link("passaggio.dat", file_messaggi);
    fclose(scrivo);
    unlink("passaggio.dat");
    pthread_mutex_unlock(fileMessagesAccess);

    if (occorrenze_messaggio == 0){ // Messaggio non presente
            return -8+1000;
        }
    if (occorrenze_messaggio == 1 && occorrenze_utente == 0){ // Messaggio presente, ma non appartiene all'utente
            return -9+1000;
        }
    return 1+1000;
}

int controllo_accesso(char *nome_utente, char *password){
	
	if(nome_utente[0] == '\n'){
		return -11+1000;
	}

	pthread_mutex_lock(fileUsersAccess);

	if(utente_loggato.id_utente_loggato != 0){
		pthread_mutex_unlock(fileUsersAccess);
		return -1+1000; // Utente già loggato
	}

    FILE *infile;
    struct utente input;
     
    infile = fopen ("user.dat", "a+");
    if (infile == NULL)
    {
		fclose(infile);
		pthread_mutex_unlock(fileUsersAccess);
        return -5+1000; // Errore nell'apertura del file
    }
    
	if( access("user.dat", F_OK ) == -1 ) {
	return -3+1000;
	}

	// Cerco occorrenze di nome utente e password nel file degli utenti
    while(fread(&input, sizeof(struct utente), 1, infile)){
		// Confronto con l'hash della password sul server
		if(strcmp((const char*)&(input.nome_utente),(const char *)nome_utente) == 0 & strcmp((const char *)&(input.password_utente), (const char *)password) == 0){
			utente_loggato.id_utente_loggato = input.id_utente;
			strcpy(utente_loggato.nome_utente_loggato, input.nome_utente);
			fclose(infile);
			pthread_mutex_unlock(fileUsersAccess);
			return input.id_utente+1000; // Restituisco il nome utente
		}
	}

	pthread_mutex_unlock(fileUsersAccess);
    fclose(infile);
    return -3+1000; // Username o password errati
}

int inserisci_nuovo_messaggio(char *messaggio, char *oggetto, char *mittente, int id_utente_loggato){
	
	if(utente_loggato.id_utente_loggato == 0){
		return -2+1000; // Utente non loggato
	}
	
	// Inserisco il nuovo messaggio
	struct messaggi nuovo_messaggio;
	nuovo_messaggio.id_utente = id_utente_loggato;
	nuovo_messaggio.id_messaggio = session.lastMessage + 1;
	strcpy(nuovo_messaggio.messaggio, messaggio);
	strcpy(nuovo_messaggio.oggetto, oggetto);
	strcpy(nuovo_messaggio.mittente, mittente);

		if(messaggio[0] == '\n' || oggetto[0] == '\n'){
		return -11+1000;
	}
	
	pthread_mutex_lock(fileMessagesAccess);
	
	FILE *outfile;
     
    outfile = fopen (file_messaggi, "a+");
    if (outfile == NULL){   
		pthread_mutex_unlock(fileMessagesAccess);
        return -5+1000; // Errore nell'apertura del file
    }
     
    if (fwrite(&nuovo_messaggio, sizeof(struct messaggi), 1, outfile) < 0){
		pthread_mutex_unlock(fileMessagesAccess);
		fclose(outfile);
		return -7+1000; // Errore nella scrittura su file
	}  
    fclose(outfile);
    pthread_mutex_unlock(fileMessagesAccess);
    pthread_mutex_lock(counter);
    session.lastMessage = session.lastMessage + 1; // Incremento il valore per la consistenza della sessione
    pthread_mutex_unlock(counter);
    return 1+1000; // Messaggio inserito correttamnte
} 

int inserisci_nuovo_utente(char *nome_utente, char *password_utente){
	
	int id_utente = session.lastUser+1;
	struct utente nuovo_utente, passaggio;	
	nuovo_utente.id_utente = id_utente;
	strcpy(nuovo_utente.nome_utente, nome_utente);
	strcpy(nuovo_utente.password_utente, password_utente);
	
	if(nome_utente[0] == '\n'){
		return -11+1000;
	}

	pthread_mutex_lock(fileUsersAccess);
	FILE *outfile;
     
    outfile = fopen ("user.dat", "a+");
    if (outfile == NULL)
    {
        printf("\nErrore nell'apertura del file\n");
        	pthread_mutex_unlock(fileUsersAccess);

        return -5+1000; // Errore nell'apertura del file
    }

    
        while(fread(&passaggio, sizeof(struct utente), 1, outfile)){
		if(strcmp((const char*)&(passaggio.nome_utente),(const char *)nome_utente) == 0){ // Nome utente già presente
			        	pthread_mutex_unlock(fileUsersAccess);

			return -10+1000; // Errore nome utente già presente
		}
	}
     
    if(fwrite(&nuovo_utente, sizeof(struct utente), 1, outfile) == 0){
		printf("\nErrore nella scrittura su file\n");
		        	pthread_mutex_unlock(fileUsersAccess);

		return -7+1000; // Errore nella scrittura su file
		} 
	pthread_mutex_lock(counter);
	session.lastUser = session.lastUser + 1; // Incremento per la consistenza della sessione
    pthread_mutex_unlock(counter);
    fclose(outfile);
	pthread_mutex_unlock(fileUsersAccess);

    return 1+1000; // Registrazione effettuata correttamente
}
 
int leggi_tutti_messaggi(void *socket){
	char buff[20];
	struct tm *sTm;
	time_t now = time (0);
	sTm = gmtime (&now);
	strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	  
    pthread_mutex_lock(fileMessagesAccess);
    FILE *infile;
    struct messaggi input;
    int valore_ritorno;
    infile = fopen (file_messaggi, "a+");
    if (infile == NULL)
    {
		pthread_mutex_unlock(fileMessagesAccess);

		return(-5+1000); // Errore nell'apertura del file
    }

	// Comunico al Client quanti byte deve leggere
    fseek(infile, 0, SEEK_END);
    int size = htons(ftell(infile));
	int nBytes = send(sock, &size, sizeof(int), 0);

	if (nBytes < 1){
		fclose(infile);
		pthread_mutex_unlock(fileMessagesAccess);

		return((int)-1);
	} else if (nBytes != sizeof(int)){
		fclose(infile);

		pthread_mutex_unlock(fileMessagesAccess);
		return((int)-1);
	}
    if (ftell(infile) == 0){
		fclose(infile);
		pthread_mutex_unlock(fileMessagesAccess);

		return(-6+1000); // Nessun messaggio presente
	}
	
	fseek(infile, 0, SEEK_SET);
    while(fread(&input, sizeof(struct messaggi), 1, infile)){
		char *toSend = encodeMessage(&input);

		if(send(sock, toSend, sizeof(struct messaggi)+4*sizeof(char) , 0) > 0){
			if (recv(sock , &valore_ritorno , sizeof(int) , 0) >0 ){
				if (ntohs(valore_ritorno) != 1){
					fclose(infile);
					pthread_mutex_unlock(fileMessagesAccess);

					return -100+1000;
				}else{
					continue;
				}		
			}else{
				fclose(infile);
				pthread_mutex_unlock(fileMessagesAccess);

				pthread_exit((int*)-1);
				}
		}else{
			fclose(infile);
			pthread_mutex_unlock(fileMessagesAccess);

			pthread_exit((int*)-1);
			}
	}
    fclose(infile);
	pthread_mutex_unlock(fileMessagesAccess);

    return 0+1000;
}
	
void *gestore_utente(void *socket){
    char data_to_send[4096];
	char buff[2000];
    struct tm *sTm;
	int scelta, valore_ritorno, uscita_thread, read_size;
	sock = *((int*)socket);

    // Codice per impostare il timeout sul socket per la connessione             
    struct timeval tv;
    tv.tv_sec = 100000;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	
    char *prova = malloc(1024*sizeof(char));
	utente_loggato.id_utente_loggato = 0;
	char *buffer = malloc(sizeof(struct comunicazione)+3*sizeof(char));
    char comunicazioneServer[1024];
	int ricevuti;
	while(ricevuti = recv(sock , buffer, sizeof(struct comunicazione)+3*sizeof(char), 0)> 0){
        // Ricevo il messaggio, ma devo comunque dare una risposta
        char out[2000];
        sprintf(out, "\nSocket numero: %d\nIl messaggio ricevuto è: %s\n", sock, buffer);
        puts(out);
        int bytes_read;
        int fd;
        char *reqline[3];

        reqline[0] = strtok(buffer, " \t\n");

		if ( strncmp(reqline[0], "GET\0", 4)==0){
			reqline[1] = strtok(NULL, " \t");
			reqline[2] = strtok(NULL, " \t\n");
        }

        if ( strncmp(reqline[1], "/\0", 2)==0){
            reqline[1] = "index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...
        } else {
            reqline[1] = strtok(reqline[1], "/");
        }

        // Invio la risposta
        if ( (fd=open(reqline[1], O_RDONLY))!=-1 )    //FILE FOUND
        {
            struct stat s;
            fstat(fd, &s);
            char responseMessage[1000];
            sprintf(responseMessage,"HTTP/1.1 200 OK\nContent-Length: %d\nLast-Modified: %s\n", s.st_size, ctime(&s.st_mtime));
            send(sock, responseMessage, strlen(responseMessage), 0);
            while ( (bytes_read=read(fd, data_to_send, 4096))>0 )
                write (sock, data_to_send, 4096);
            close(fd);
        } else{
			struct stat s;
			fd=open("404.html", O_RDONLY);
            fstat(fd, &s);
            char responseMessage[1000];
            sprintf(responseMessage,"HTTP/1.1 404 Not Found\nContent-Length: %d\nLast-Modified: %s\n", s.st_size, ctime(&s.st_mtime));
            send(sock, responseMessage, strlen(responseMessage), 0);
			sprintf(data_to_send, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL /%s was not found on this server.</p></body></html>", reqline[1]);
			write (sock, data_to_send, 4096);
            close(fd);
        }
    }
	
	time_t now = time (0);
	sTm = gmtime (&now);
	strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	free(buffer);

	pthread_mutex_lock(counter);
	session.utenti_connessi = session.utenti_connessi -1 ;
	pthread_mutex_unlock(counter);

	sprintf(comunicazioneServer, "%s | Socket numero: %d | Connessione interrotta. Utenti connessi: %d", buff, sock, session.utenti_connessi);
	puts(comunicazioneServer);
	close(sock);
	pthread_exit((int*)-1);
}

int visualizza_utenti(){
	FILE *infile;
    struct utente input;
    
    infile = fopen (file_utenti, "r");
    if (infile == NULL)
    {
		return(-5);
    }
    printf("\nID Utente | Nome utente | Password utente\n");
    while(fread(&input, sizeof(struct utente), 1, infile)){
        printf ("\n%d | %s | %s\n", input.id_utente, input.nome_utente, input.password_utente);
    }
    fclose(infile);
    printf("\n");
    return 1;
}
	
void help(){
	printf("\nUtilizzo: Server [opzioni]\n\nOpzioni:\n  numero_porta                                    Avvia il server sulla porta indicata\n  --seeuser                                       Permette di visualizzare la lista degli utenti presenti nel sistema\n\n");
}

static void *sig_thread(void *arg)
{
    sigset_t *set = arg;
    int s, sig;
	
        s = sigwait(set, &sig);
        if (s != 0)
			exit(-1);

		pthread_mutex_lock(fileMessagesAccess);
		pthread_mutex_lock(fileUsersAccess);
		pthread_mutex_lock(counter);
		printf("\n\nProcedo con la distruzione dei mutex...\n");
		pthread_mutex_destroy(fileMessagesAccess);
		pthread_mutex_destroy(fileUsersAccess);
		pthread_mutex_destroy(counter);
		printf("\nTermino l'esecuzione...\n");
		exit(-2);    
	
}

int main(int argc , char *argv[]){			
	int porta;

	if (argc > 2 || argc == 1){

			printf("\nUtilizzo: Server [opzioni]\n\nOpzioni:\n  indirizzo                    Avvia il server utilizzando la porta indicata \n  --seeuser                    Mostra la lista degli utenti registrati\n");
			return 0;
		}
	
	if (argc == 2){
		if (strcmp(argv[1], "--seeuser") == 0){
			visualizza_utenti();
			return 0;
	} 
	
	else if(strcmp(argv[1], "--help") == 0){
		help();
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

	sigset_t set;
	sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
	signal(SIGPIPE, SIG_IGN);
	pthread_t threadSegnali;
	pthread_create(&threadSegnali, NULL, &sig_thread, (void *) &set);


	char buff[20];
    struct tm *sTm;
    int socket_desc , *socket_cliente , c , read_size;
	int utenti_connessi = 0;
    struct sockaddr_in server , client;
    char client_message[2000];
    pthread_t thread, thread1, thread2;
    /* pthread_create(&thread2, NULL, recupero_consistenza_file, NULL);
	pthread_join(thread2, NULL);
	pthread_create(&thread1, NULL, recupero_consistenza_sessione, NULL);
    pthread_join(thread1, NULL);
    */

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
	time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
		
	socket_cliente = malloc(sizeof(int));
    *socket_cliente = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    
		if (*socket_cliente < 0)
		{
			perror("Connessione rifiutata.");
			return 1;
		}
		
		pthread_mutex_lock(counter);
		session.utenti_connessi = session.utenti_connessi + 1;
		pthread_mutex_unlock(counter);

		time_t now = time (0);
		sTm = gmtime (&now);
		strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
		
		char comunicazioneConnessione[1024];
		sprintf(comunicazioneConnessione, "\n%s | Socket numero: %d | Nuova connessione accettata. Utenti connessi: %d", buff, *socket_cliente, session.utenti_connessi);
		puts(comunicazioneConnessione);
		
		pthread_create(&thread, NULL, gestore_utente, (void*)socket_cliente);
    }
    return 0;
}