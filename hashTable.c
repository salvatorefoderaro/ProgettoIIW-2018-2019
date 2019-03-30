#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedListImage.c"

#define SIZE 30

struct DataItem {
    treeNode *imageList;
   int data;   
   long key;
};

struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

char *searchHash(char *string, int w, int h, int quality, int colorSpace, int *size);
void insertHash(char *string, treeNode *imageList);
struct DataItem* deleteHash(struct DataItem* item);

signed long hash(char *str)
    {
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
   long key = hash(string);
   int hashIndex = hashCode(key);  
	
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key)
         printf("Address is: %x", hashArray[hashIndex]->imageList);
      	return searchNodeInList(hashArray[hashIndex]->imageList, string, w, h, quality, colorSpace, size); 
			
      ++hashIndex;
		
      hashIndex %= SIZE;
   }        

    treeNode *imageListNode = createNode(string, 1024, 1024, 0, 0);
    insertHash(string, imageListNode);
    size = &(imageListNode->size);
    return imageListNode->imageBuffer;
}

void insertHash(char *string, treeNode *imageList) {

    long key = hash(string);

   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->data = NULL;
   item->imageList = imageList;
   item->key = key;

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

   printf("\nAddress in Hash is: %x %x\n", test, test1);

   FILE *write_ptr;
   write_ptr = fopen("test12422221.jpg","wb");
   fwrite(test,12576,1,write_ptr);
}