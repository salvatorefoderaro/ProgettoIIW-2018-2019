#include <stdio.h>
#include <stdbool.h>
#include "imageToBlob.h"
#include "newHash.h"
#include "coda.h"
#include "config.h"

nodo *testaCoda;
hashNode *testaHash[SIZE] = { NULL }; 

long hashCode(long key) { //calcolo indice per la tabella hash
   return key % SIZE;
}

unsigned long hash(unsigned char *str){
	unsigned int hash = 0;
	int c;
	while (c = *str++)
	    hash += c;
	return hash;
    }

int deleteHashNode(long hashIndexDelete){

    int hashIndex = hashCode(hashIndexDelete);
    int size;

    if (testaHash[hashIndex]->key == hashIndexDelete){//posso mettere i primi due if in un unico mettendo testaHash[hashIndex] = testaHash[hashIndex]->next; 

        hashNode *toDelete = testaHash[hashIndex];
        testaHash[hashIndex] = testaHash[hashIndex]->next;
        size = toDelete->imageSize;
        MagickRelinquishMemory(toDelete->imageBuffer);
        pthread_mutex_destroy((pthread_mutex_t*)toDelete->sem);
        free(toDelete);
        return size;

    } else {

        hashNode *support = testaHash[hashIndex];
        while(support->next->key != hashIndexDelete){
            if(support->next->next==NULL){ //se il nodo testa->suc non e' quello cercato e il nodo successivo e' NULL => il nodo ricercato non e' nella lista
		        perror("\nNodo non presente nella tabella Hash\n");
                return 0; //perche' non ho eliminato niente
            }
            support = support->next;
        }
        size = support->imageSize;
        MagickRelinquishMemory(support->next->imageBuffer);  
        pthread_mutex_destroy((pthread_mutex_t*)support->next->sem);
        free(support->next);
        support->next = support->next->next;
        return size;
    }
}

hashNode *searchHashNode(char *string, int w, int h, int quality, int *toSend, char *fileType){

    // Alloco lo spazio per una stringa contenente il nome del file ed i parametri
    char *toHash = calloc(1, 4*sizeof(int) + 4*sizeof(char)+strlen(string)); // Controllo sul Null per la malloc
    if(!toHash){
		perror("\nErrore nella funzione malloc()!\n");
        free(toHash);
        pthread_exit((void*)-1);
    } 
    
    sprintf(toHash, "%s|%d|%d|%d", string, w, h, quality);
    
    // Converto la stringa generata in codice hash
    unsigned long key = hash(toHash);

    // Ottengo l'indice della tabella dal codice generato in precedenza
    int hashIndex = hashCode(key);

    // Scorro la tabella Hash per cercare se il nodo è presente
    hashNode *support = testaHash[hashIndex];
    hashNode *lastNode = support;
    int c;
    while(support != NULL){
        c=0;
        if (support->key == key){
            if(pthread_rwlock_tryrdlock(support->sem)!=0){ 
            //if fail->
               support=testaHash[hashIndex]; //cosi' rifa' il controllo del nodo in cache(cioe' rifa' la read) perche' il semaforo e' in scrittura quindi il nodo e' in fase di eliminazione
               c=1; //se capita che testaHash[hashIndex]->key=key (e quindi facendo->next non ci sarebbe un controllo e si avrebbe un eventuale inserimento)    
            }//else: pthread_rwlock_rdlock(&(support->sem)); dovrebbe gia' prenderlo
            else{
              toSend = &(support->imageSize);
              inserisci_n(testaCoda,key,support);
              free(toHash);
              return support;
            }
        }
        if (c==0){
            if(support->next == NULL){
                lastNode = support;
            }
            support = support->next;
        }
    }

    // Se il nodo non è stato trovato...

    // Alloco memoria per la creazione del nuovo nodo
    hashNode *testNode = malloc(sizeof(hashNode));
    if(!testNode){
		perror("\nErrore nella funzione malloc()!\n");
     free(toHash);
     pthread_exit((void*)-1);
    } 
    testNode->next = NULL;
    testNode->key = key;
    testNode->imageBuffer = getBlob(string, w, h, quality, toSend, fileType);
    testNode->imageSize = *toSend;
    testNode->sem = malloc(sizeof(pthread_rwlock_t));
    pthread_rwlock_init(testNode->sem, NULL);
    pthread_rwlock_rdlock(testNode->sem);

    // Controllo che sia disponibile memoria per l'immagine che voglio andare ad inserire...
    //non serve il semaforo in scrittura per il nuovo nodo perche' nessuno puo' leggerlo o scriverci perche' non il nodo non e' ancora inserita nella hash

    //open sem_nominato(hashIndex,0)
    //if fail(vuol dire che qualcun' altro sta inserendo lo stesso nodo)->libera il nodoHash allocato e return NULL(ci vorrebbe un goto) (con NULL bisogna mettere un controllo nel server per cui se ritorna null rileggo)
    //if ok->inserisco il nodo e distruggo il semaforo nominato 
    sem_t* se=sem_open(toHash,O_CREAT,0666,0);      
    if(se == SEM_FAILED && errno==EEXIST){ //mettere l'errore 
        free(testNode);
        free(toHash);
        return NULL; //restituendo NULL il server rifara' la lettura  
    }
  
    if (testNode->imageSize > dimensioneTotale){
        free(toHash);
        return testNode;
    }

    // Effettuo l'inserimento del nodo nella tabella Hash e nella coda con priorita'
    while( (limite_dimensione - testNode->imageSize) < 0 ){
        
        // ... in caso contrario elimino la testa dalla coda con priorità per liberare spazio

        testaCoda = libera_n(testaCoda, testaCoda);//lui elimina solo il nodo della coda con priorita' e non il nodo nella cache
        //libera_n(testaCoda,testaCoda); questa in piu' aggiorna la dimensione ed elimina il nodo . Dovrei passargli il nodo 

    }
    insertHashNode(testNode, lastNode, hashIndex);
    
    limite_dimensione-=testNode->imageSize;
    sem_unlink(toHash);
    sem_destroy(se);   
    free(toHash);
    return testNode; //testNode->imageBuffer;
}

void insertHashNode(hashNode *newNode, hashNode *lastNode, int index){

    // Effettuo l'inserimento nella coda con priorità
    testaCoda = inserisci_n(testaCoda, newNode->key, newNode);
    // Controllo se è da efettuare l'inserimento in testa...
    if (testaHash[index] == NULL){
        testaHash[index] = newNode;
        return;

    // ... oppure in coda all'indice della tabella Hash
    } else {
        if(lastNode != NULL)
            lastNode->next = newNode;
        return;
    }
}

/* int main(void){

    MagickWandGenesis();

    testaCoda = NULL;

    int *sizeWe = malloc(sizeof(int));

    searchHashNode("AAA1", 10, 10, 10, 10, sizeWe);
    searchHashNode("AAA1", 10, 10, 10, 10, sizeWe);
    searchHashNode("BBBB", 10, 10, 10, 10, sizeWe);
    searchHashNode("BBBB", 10, 10, 10, 10, sizeWe);
    searchHashNode("AAA", 10, 10, 10, 10, sizeWe);
    searchHashNode("AAA", 10, 10, 10, 10, sizeWe);

    stampa(testaCoda);
    testaCoda = libera_n(testaCoda, testaCoda);
    printf("Testa coda is: %x", testaCoda);
    stampa(testaCoda);
    MagickWandTerminus();
} */