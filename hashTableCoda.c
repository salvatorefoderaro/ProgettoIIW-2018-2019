#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "imageToBlob.c"
#include "hashTableCoda.h"
#include "codaHash.h"

#define SIZE 30

DataItem* hashArray[SIZE]; 
DataItem* dummyItem;
DataItem* item;
nodo *testa;

char *searchHash(char *string, int w, int h, int quality, int colorSpace);
void insertHash(char *string, char *imageBuffer, int imagesize);
struct DataItem* deleteHash(struct DataItem* item);

signed long hash(char *str){
    signed long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash * 33) + hash) + c;

    return hash;
}

long hashCode(long key) {
   return key % SIZE;
}

char *searchHash(char *string, int w, int h, int quality, int colorSpace){

   char *intToString = malloc(4*sizeof(int) + 4*sizeof(char));
   sprintf(intToString, "|%d|%d|%d|%d", w, h, quality, colorSpace);
   
   char *toHash = malloc(4*sizeof(int) + 4*sizeof(char) + strlen(string));
   memset(toHash, 0, 4*sizeof(int) + 4*sizeof(char) + strlen(string));
   strcat(toHash, string);
   strcat(toHash, intToString);
   
   long key = hash(toHash);
   int hashIndex = hashCode(key);

   free(intToString);
	
    while(hashArray[hashIndex] != NULL) {

    if(hashArray[hashIndex]->key == key){
        printf("\n     *****     Nodo presente nella tabella Hash     *****     \n");
        testa = inserisci_n(testa,hashArray[hashIndex]->queue);
        return hashArray[hashIndex]->imageBuffer; 
    }
    ++hashIndex;
    hashIndex %= SIZE;
   }        
   int imageSize;
    printf("\n     *****     Nodo non presente nella tabella Hash, procedo con l'inserimento     *****\n");
    char *image = getBlob(string, w, h, quality, colorSpace, &imageSize);
    insertHash(toHash, image, imageSize);
    return image;
}

void insertHash(char *string, char *image, int imageSize) {

    long key = hash(string);
    free(string);

    struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
    item->data = NULL;
    item->key = key;
    item->imageBuffer = image;
    item->imageSize = imageSize;
   
    struct nodo *node = malloc(sizeof(struct nodo));
    node -> hashItem = item;
    strcpy(node->indice, string);
    item->queue = node;
    testa = inserisci_n(testa, node);

    int hashIndex = hashCode(key);

    while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
        ++hashIndex;
        hashIndex %= SIZE;
    }
	
    hashArray[hashIndex] = item;
}

struct DataItem* deleteHash(struct DataItem* item) {
   
   int key = item->key;
   int hashIndex = hashCode(key);

   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key) {
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
   dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
   dummyItem->data = -1;  
   dummyItem->key = -1; 

   char *test = searchHash("ciao", 300, 300, 0, 0);
   char *test1 = searchHash("ciao", 1000, 10000, 0, 0);
   printf("%x\n", searchHash("ciao", 1000, 10000, 0, 0));

   FILE *write_ptr;
   write_ptr = fopen("test12422221.jpg","wb");
   fwrite(test,12576,1,write_ptr);
   
   printf("Testa is: %s", testa);
   stampa(testa);
}