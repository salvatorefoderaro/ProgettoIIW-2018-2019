#ifndef _HASHTABLECODA_H 
#define _HASHTABLECODA_H

struct DataItem {
   int data;   
   long key;
   char *imageBuffer;
   int imageSize;
   pthread_rwlock_t *test;
};

struct DataItemHead {
   long key;
   struct DataItem *next;
}

typedef struct DataItem DataItem;

#define SIZE 5

#endif