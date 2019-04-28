#ifndef _NEWHASH_H 
#define _NEWHASH_H

struct hashNode {
   long key;
   char *imageBuffer;
   struct hashNode *next;
   int imageSize;
};

typedef struct hashNode hashNode;
#define SIZE 2

extern int limite_dimensione;  /* Declaration of the variable */

void insertHashNode(hashNode *newNode, int index);
int deleteHashNode(long test);
char* searchHashNode(char *string, int w, int h, int quality, int colorSpace);

#endif