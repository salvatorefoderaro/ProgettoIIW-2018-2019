#ifndef _HASHTABLECODA_H 
#define _HASHTABLECODA_H
#define dimensione_max=500000000;
long limite_dimensione=0;

struct DataItem {
   int data;   
   long key;
   char *imageBuffer;
   int imageSize;
   struct nodo *queue;
};

typedef struct DataItem DataItem;

#define SIZE 30

#endif