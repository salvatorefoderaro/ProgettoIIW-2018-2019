#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedListImage.c"
#include "pairheap.h"

#define SIZE 30

struct DataItem {
   int data;   
   long key;
   char *imageBuffer;
};

struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

char *searchHash(char *string, int w, int h, int quality, int colorSpace, int *size);
void insertHash(char *string, char *imageBuffer);
struct DataItem* deleteHash(struct DataItem* item);

signed long hash(char *str){
    signed long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash * 33) + hash) + c; /* hash * 33 + c */

    return hash;
}

long hashCode(long key) {
   return key % SIZE;
}

char *searchHash(char *string, int w, int h, int quality, int colorSpace, int *size) {
     
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
        return hashArray[hashIndex]->imageBuffer; 
    }
    ++hashIndex;
    hashIndex %= SIZE;
   }        

    printf("\n     *****     Nodo non presente nella tabella Hash, procedo con l'inserimento     *****\n");

    char *image = getBlob(string, w, h, quality, colorSpace, size);
    insertHash(toHash, image);
    return image;
}

void insertHash(char *string, char *image) {

    long key = hash(string);
    free(string);

    struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
    item->data = NULL;
    item->key = key;
    item->imageBuffer = image;

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

   dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
   dummyItem->data = -1;  
   dummyItem->key = -1; 

   int *lunghezza, *lunghezza1;
   char *test = searchHash("ciao", 0, 0, 0, 0, lunghezza);
   char *test1 = searchHash("ciao", 1000, 10000, 0, 0, lunghezza);
   printf("%x\n", searchHash("ciao", 1000, 10000, 0, 0, lunghezza));

   //printf("\nAddress in Hash is: %x %x\n", test, test1);

   FILE *write_ptr;
   write_ptr = fopen("test12422221.jpg","wb");
   fwrite(test,12576,1,write_ptr);
}