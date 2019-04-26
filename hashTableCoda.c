#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "imageToBlob.h"
#include "hashTableCoda.h"
#include "coda.h"

DataItem* hashArray[SIZE]; 
DataItem* dummyItem;
DataItem* item;
nodo *testa;
nodo *coda;



void insertHash(char *string, char *image, int imageSize);

signed long hash(char *str){
    signed long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash * 33) + hash) + c;
    return hash;
}

long hashCode(long key) {
   // Questa funzione prende in input un numero e ne restituisce il modulo
   // relativo alla dimensione della tabella Hash impostata
   return key % SIZE;
}

char *searchHash(char *string, int w, int h, int quality, int colorSpace){
   // Questa funzione effettua la ricerca di una stringa all'interno della tabella hash

   // Costruisco la stringa, utilizzando il nome del file da cercare e tutti quanti
   // i parametri relativi all'immagine
   char *toHash = malloc(4*sizeof(int) + 4*sizeof(char)+strlen(string));
   sprintf(toHash, "%s|%d|%d|%d|%d", string, w, h, quality, colorSpace);
   
   /* char *toHash = malloc(4*sizeof(int) + 4*sizeof(char) + strlen(string));
   memset(toHash, 0, 4*sizeof(int) + 4*sizeof(char) + strlen(string));
   strcat(toHash, string);
   strcat(toHash, intToString); */
   
   // Ottengo l'indice della stringa e ne faccio il modulo relativo alla mia tabella Hash
   long key = hash(toHash);
   int hashIndex = hashCode(key);
	
    // Scorro tutta la tabella Hash
    while(hashArray[hashIndex] != NULL) {
   
      // Controllo che il nodo che sto cercando sia presente nella tabella
      if(hashArray[hashIndex]->key == key){

         // Se è presente effettuo l'inserimento nella coda con priorità, in modo
         // da modificarne la posizione nella coda e restituisco l'indirizzo al buffer
         // dove è contenuta l'immagine
         printf("\n     *****     Nodo presente nella tabella Hash     *****     \n");
         testa = inserisci_n(testa, toHash);
         return hashArray[hashIndex]->imageBuffer; 
    }
    ++hashIndex;
    hashIndex %= SIZE;
   }        
   /*
   Se invece il nodo non è resente nella lista... Inizializzo l'intero "imageSize"
   per poter fare il successivo controllo con la dimensione disponibile in tabella
   */
   int imageSize;
   printf("\n     *****     Nodo non presente nella tabella Hash, procedo con l'inserimento     *****\n");
   
   /*
   Utilizzo la funzione getBlob per caricare l'immagine con i parametri desiderati
   in memoria ed ottenere come valore di ritorno "image" l'indirizzo del buffer
   */

   char *image = getBlob(string, w, h, quality, colorSpace, &imageSize);
  
   // Effettuo l'inserimento della nuova immagine/versione nella tabella Hash
   insertHash(key, image, imageSize);

   return image;
}

void insertHash(long key, char *image, int imageSize) {

   // Creo il nodo per la tabella Hash ed effettuo l'inserimento
   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->data = NULL;
   item->key = key;
   item->imageBuffer = image;
   item->imageSize = imageSize;
   item->test = malloc(sizeof(pthread_rwlock_t));
   pthread_rwlock_init(item->test);
   
   testa = inserisci_n(testa, string);

   int hashIndex = hashCode(key);

   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
      ++hashIndex;
      hashIndex %= SIZE;
   }

   hashArray[hashIndex] = item;
   free(string);
}

   /* Funzione da sistemare */
struct DataItem* deleteHash(struct DataItem* item) {
   /*
   Elimino un nodo dalla tabella Hash ma è tutto da ricontrollare questo
   in quanto non mi sembra che faccia la free del nodo precedentemente allocato,
   ma si limita ad impostare un "dummyItem" che alcun senso non ha!
   */

   int key = item->key;
   int hashIndex = hashCode(key);

   while(hashArray[hashIndex] != NULL) {
      if(hashArray[hashIndex]->key == key) {
         pthread_rwlock_rdlock(hashArray[hashIndex]->test);
         struct DataItem* temp = hashArray[hashIndex]; 
         hashArray[hashIndex] = dummyItem; 
         return temp;
      }	
      ++hashIndex;
      hashIndex %= SIZE;
   }      
   return NULL;        
}

void display() {
   int i = 0;
	
   for(i = 0; i<SIZE; i++) {
      if(hashArray[i] != NULL)
         printf(" (%ld,%d)",hashArray[i]->key,hashArray[i]->data);
      else
         printf(" ~~ ");
   }
   printf("\n");
}

int main() {
   testa = NULL;
   coda = NULL;

   /*
   dummyItem che serve per l'eliminazione, ma boh.
   */
   dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
   dummyItem->data = -1;  
   dummyItem->key = -1; 

   printf("\n\n");
   display();
   printf("\n\n");

   char *test = searchHash("template.jpg", 0, 0, 0, 0);
   char *test1 = searchHash("template.jpg", 1000, 10000, 0, 0);
   char *test2 = searchHash("template.jpg", 1000, 10000, 0, 0);
   
   printf("\n     *****     Stampa della coda con priorita'     *****\n");
   stampa(testa);
   // testa = libera_n(testa, testa); # Prova per vedere se funziona la rimozione della testa
   // stampa(testa);

   /*
   Va aggiunto come parametro di input l'indirizzo dell'intero che indica la dimensione
   in questo modo posso passarlo al Server che può mettere nell'header la lunghezza
   */
  
   // FILE *write_ptr;
   // write_ptr = fopen("test12422221.jpg","wb");
   // fwrite(test,5000,1,write_ptr);
}