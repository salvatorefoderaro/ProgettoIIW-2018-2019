/****************************************************************

SPECIFICATION TO BE IMPLEMENTED:
Implementare un programma che riceva in input tramite argv[] i pathname 
associati ad N file, con N maggiore o uguale ad 1. Per ognuno di questi
file generi un processo che legga tutte le stringhe contenute in quel file
e le scriva in un'area di memoria condivisa con il processo padre. Si 
supponga per semplicita' che lo spazio necessario a memorizzare le stringhe
di ognuno di tali file non ecceda 4KB. 
Il processo padre dovra' attendere che tutti i figli abbiano scritto in 
memoria il file a loro associato, e successivamente dovra' entrare in pausa
indefinita.
D'altro canto, ogni figlio dopo aver scritto il contenuto del file nell'area 
di memoria condivisa con il padre entrera' in pausa indefinita.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo padre venga colpito da esso dovra' 
stampare a terminale il contenuto corrente di tutte le aree di memoria 
condivisa anche se queste non sono state completamente popolate dai processi 
figli.

*****************************************************************/


#define _REENTRANT
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4096
#define Error_(x,y)	{ puts(x); exit(y); }


int 	sem_id,		num_proc;	

char	**file_names,	**shared_buffers;



void generic_handler( int signo){

	int ret;

	sigset_t set;
	sigemptyset(&set);
	sigaddset( &set, SIGINT);
	sigprocmask( SIG_BLOCK, &set, NULL);

	for (int i = 0; i < num_proc; i ++) {
		printf("\n\n contents of shared_buffer related on file '%s' are:\n ", file_names[i]);
		printf("%s", shared_buffers[i]);	
	}
	
	exit(0);
}



void child_function( int ind);



void main( int argc, char **argv) {

	if (argc < 2)		Error_("too few args in funct: main. please provide at least an existing file name.", 1);
	
	int ret;

	//temporarily block SIGINT occurrences.
	sigset_t set;
	sigemptyset( &set);
	sigaddset( &set, SIGINT);
	sigprocmask( SIG_BLOCK, &set, NULL);
	
	//store command line args into global variables.
	num_proc = argc-1;

	file_names = malloc( sizeof( char*) * num_proc);
	if(file_names == NULL)		Error_("error in funct: malloc", 1);

	for ( int i = 0; i < num_proc; i ++) {
		file_names[i] = malloc( sizeof(char) * strlen(argv[i + 1]));
		if (file_names[i] == NULL)		Error_("error in funct: malloc", 1);
		ret = sprintf( file_names[i], "%s", argv[i + 1]);
		if (ret == -1)		Error_("error in funct: sprintf", 1);
	}	

	//validate memory buffers shared by parent and child processes.
	shared_buffers = malloc( sizeof(char*) * num_proc);
	if (shared_buffers == NULL)		Error_("error in funct: malloc", 1);

	for (int i = 0; i < num_proc; i ++) {
		shared_buffers[i] = mmap( NULL, SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
		if (shared_buffers[i] == (void *) -1)		Error_("error in funct: mmap", 1);
	}


	//allocate and initiate semaphore structures.
	sem_id = semget( IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0660);
	if (sem_id == -1)		Error_("error in funct: semget", 1);
	
	ret = semctl( sem_id, 0, SETVAL, 0);
	if (ret == -1)		Error_("error in funct: malloc", 1);

	//create all child processes and launch their activities.
	for (int i = 0; i < num_proc; i ++) {
		ret = fork();
		if (ret == -1)		Error_("error in funct: fork", 1);

		if (ret == 0) {
			//child operations.
			child_function( i);
			exit(0);
		}
	}


	signal( SIGINT, generic_handler);
	sigprocmask( SIG_UNBLOCK, &set, NULL);

	struct sembuf oper;

	oper.sem_op = - num_proc;
	oper.sem_num = 0;
	oper.sem_flg = 0;
	ret = semop( sem_id, &oper, 1);
	if (ret == -1)		Error_("error in funct: semop (parent)", 1);

	do{
		pause();	
	
	} while(1);


	return;

}





void child_function( int ind){

	int 	ret;	ssize_t len = 0;

	struct sembuf oper;	FILE *f;
	
	//just terminate when SIGINT occurs.
	sigset_t set;
	sigemptyset( &set);
	sigaddset( &set, SIGINT);
	sigprocmask( SIG_IGN, &set, NULL);	
	
	f = fopen( file_names[ind], "r");
	if (f == NULL)		Error_("error in funct: fopen (child)", 1);


	char* line = NULL;	char* next = NULL;	char* tmp = shared_buffers[ind];

	while ( getline( &line, &len, f) ) {

		printf("\nprocess %d is getting line!\n", getpid());
			
		next = strtok( line, ' ');
		sprintf( shared_buffers[ind], "%s ", line);
		tmp += strlen(shared_buffers[ind]);

		do{
			printf("\nprocess %d is tokening!\n", getpid());
			sprintf( tmp, "%s ", next);
			next = strtok( NULL, ' ');
			tmp += strlen(shared_buffers[ind]);
			
		} while( next != NULL);
		
	
	}


	oper.sem_op = 1;
	oper.sem_num = 0;
	oper. sem_flg = 0;
	ret = semop( sem_id, &oper, 1);
	if (ret == -1)		Error_("error in funct: semop (child)", 1);


	do{
		pause();
	
	} while(1);



}