#ifndef _NEWHASH_H 
#define _NEWHASH_H
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
struct hashNode {
   long key;
   char *imageBuffer;
   struct hashNode *next;
   int imageSize;
   pthread_rwlock_t *sem; 
};

typedef struct hashNode hashNode;

extern int limite_dimensione;  /* Declaration of the variable */

void insertHashNode(hashNode *newNode, hashNode *lastNode, int index);
int deleteHashNode(long test);
hashNode* searchHashNode(char *string, int w, int h, int quality, int *toSend, char *fileType);

#endif