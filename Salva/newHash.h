#include <stdio.h>
#include <stdbool.h>
#include "imageToBlob.h"
#include "coda.h"

nodo *testa;

struct hashNode {
   long key;
   char *imageBuffer;
   struct hashNode *next;
   int imageSize;
};

typedef struct hashNode hashNode;
#define SIZE 2
hashNode *testa[SIZE] = { NULL }; 