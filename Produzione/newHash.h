#ifndef _NEWHASH_H 
#define _NEWHASH_H
#include <pthread.h>
struct hashNode {
   long key;
   char *imageBuffer;
   struct hashNode *next;
   int imageSize;
   pthread_rwlock_t sem; 
};

typedef struct hashNode hashNode;
#define SIZE 2

extern int limite_dimensione;  /* Declaration of the variable */

void insertHashNode(hashNode *newNode, int index);
int deleteHashNode(long test);
hashNode* searchHashNode(char *string, int w, int h, int quality, int colorSpace, int *toSend);

#endif