#include <stdio.h>
#include <stdbool.h>
#include "imageToBlob.h"
#include "newHash.h"
#include "coda.h"

nodo *testaCoda;
hashNode *testaHash[SIZE] = { NULL }; 

long hashCode(long key) {
   return key % SIZE;
}

signed long hash(char *str){
    signed long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash * 33) + hash) + c;
    return hash;
}

int deleteHashNode(long test){

    int hashIndex = hashCode(test);
    int size;

    if (testaHash[hashIndex]->key == test && testaHash[hashIndex]->next != NULL){//posso mettere i primi due if in un unico mettendo testaHash[hashIndex] = testaHash[hashIndex]->next; 

        printf("Entro nel primo?");
        hashNode *toDelete = testaHash[hashIndex];
        testaHash[hashIndex] = testaHash[hashIndex]->next;
        size = toDelete->imageSize;
        MagickRelinquishMemory(toDelete->imageBuffer);
        free(toDelete);
        return size;

    } else if (testaHash[hashIndex]->key == test && testaHash[hashIndex]->next == NULL){

        printf("Entro nel secondo?");
        hashNode *toDelete = testaHash[hashIndex];
        size = toDelete->imageSize;
        MagickRelinquishMemory(toDelete->imageBuffer);
        free(toDelete);
        testaHash[hashIndex] = NULL;
        return size;

    } else {

        hashNode *support = testaHash[hashIndex];
        while(support->next->key != test){

            support = support->next;
        }

        if (support->next->next != NULL){ //aggregare i due if?

            hashNode *toDelete = support->next;
            size = toDelete->imageSize;
            MagickRelinquishMemory(toDelete->imageBuffer);  
            free(toDelete);
            support->next = support->next->next;
            return size;

        } else {

            size = support->next->imageSize;
            MagickRelinquishMemory(support->imageBuffer);
            free(support->next);
            support->next = NULL;
            return size;

        }
    }
}

hashNode * searchHashNode(char *string, int w, int h, int quality, int colorSpace, int *toSend){

    // Alloco lo spazio per una stringa contenente il nome del file ed i parametri
    char *toHash = malloc(4*sizeof(int) + 4*sizeof(char)+strlen(string));
    sprintf(toHash, "%s|%d|%d|%d|%d", string, w, h, quality, colorSpace);
    
    // Converto la stringa generata in codice hash
    long key = hash(toHash);

    // Ottengo l'indice della tabella dal codice generato in precedenza
    int hashIndex = hashCode(key);
    printf("\nHash index is: %d\n", hashIndex);

    // Scorro la tabella Hash per cercare se il nodo è presente
    hashNode *support = testaHash[hashIndex];
    int c;
    while(support != NULL){
        c=0;
        if (support->key == key){
            //try to read sem
            if(pthread_rwlock_tryrdlock(&(support->sem))!=0){ 
            //if fail->
               support=testaHash[hashIndex]; //cosi' rifa' il controllo del nodo in cache(cioe' rifa' la read) perche' il semaforo e' in scrittura quindi il nodo e' in fase di eliminazione
               c=1; //se capita che testaHash[hashIndex]->key=key (e quindi facendo->next non ci sarebbe un controllo e si avrebbe un eventuale inserimento)    
            }//else: pthread_rwlock_rdlock(&(support->sem)); dovrebbe gia' prenderlo
            else{
              toSend = &(support->imageSize);
              printf("\nTo send is: %d\n", *toSend);
              printf("\nFound\n");
           
                 //IMPORTANTE aggiungere aggiornamento della coda con priorita' in quanto il nodo e' stato appena acceduto e quindi ha piu' priorita'
              inserisci_n(testaCoda,key,support) //quando inserisco ho il problema: (struct DataItem *hashItem), cioe' se sostituisco il nodo devo passare anche il puntatore al DataItem     
              return support//support->imageBuffer;
            }
        }
        if (c==0){
          support = support->next;
        }
        
    }

    // Se il nodo non è stato trovato...

    // Alloco memoria per la creazione del nuovo nodo
    hashNode *testNode = malloc(sizeof(hashNode));
    testNode->next = NULL;
    testNode->key = key;
    testNode->imageBuffer = getBlob(string, w, h, quality, colorSpace, toSend);
    testNode->imageSize = *toSend;
    printf("\nTo send is: %d\n", *toSend);

    // Controllo che sia disponibile memoria per l'immagine che voglio andare ad inserire...
    //non serve il semaforo in scrittura per il nuovo nodo perche' nessuno puo' leggerlo o scriverci perche' non il nodo non e' ancora inserita nella hash


    //open sem_nominato(hashIndex,0)
    //if fail(vuol dire che qualcun' altro sta inserendo lo stesso nodo)->libera il nodoHash allocato e return NULL(ci vorrebbe un goto) (con NULL bisogna mettere un controllo nel server per cui se ritorna null rileggo)
    //if ok->inserisco il nodo e distruggo il semaforo nominato 
    se=sem_open(toHash,O_CREAT,0666,0);      
    if(errno==EEXIST){ //mettere l'errore 
        free(testNode);
        return NULL; //restituendo NULL il server rifara' la lettura  
    }
  
    // Effettuo l'inserimento del nodo nella tabella Hash e nella coda con priorita'
    while( (limite_dimensione - testNode->imageSize) < 0 ){
        
        // ... in caso contrario elimino la testa dalla coda con priorità per liberare spazio

        libera_ln(testaCoda);//lui elimina solo il nodo della coda con priorita' e non il nodo nella cache
        //libera_n(testaCoda,testaCoda); questa in piu' aggiorna la dimensione ed elimina il nodo . Dovrei passargli il nodo 


    }
    insertHashNode(testNode, hashIndex);
    
    limite_dimensione-=testNode->imageSize;
    sem_unlink(toHash);
    sem_destroy(se);   
         
    return testNode//testNode->imageBuffer;
}

void insertHashNode(hashNode *newNode, int index){

    // Effettuo l'inserimento nella coda con priorità
    testaCoda = inserisci_n(testaCoda, newNode->key);

    // Controllo se è da efettuare l'inserimento in testa...
    if (testaHash[index] == NULL){

        testaHash[index] = newNode;
        return;

    // ... oppure in coda all'indice della tabella Hash
    } else {

        hashNode *support = testaHash[index];

        while(support->next != NULL){
            if (support->next->key == newNode->key){
                return
            }
            support = support->next;
        }
        
        support->next = newNode;
        return;
    }
}

/* int main(void){

    MagickWandGenesis();

    testaCoda = NULL;

    int *sizeWe = malloc(sizeof(int));

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("AAA1", 10, 10, 10, 10, sizeWe);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("AAA1", 10, 10, 10, 10, sizeWe);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("BBBB", 10, 10, 10, 10, sizeWe);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("BBBB", 10, 10, 10, 10, sizeWe);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);

    // printf("\nLa dimensione eliminata è: %d\n", deleteHashNode(4247311488675142450));
    
    searchHashNode("AAA", 10, 10, 10, 10, sizeWe);
    searchHashNode("AAA", 10, 10, 10, 10, sizeWe);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    stampa(testaCoda);
    libera_ln(testaCoda);
    MagickWandTerminus();
} */