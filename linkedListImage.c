#include <stdlib.h>
#include <stdio.h>

typedef struct treeNode 
{ 
  char *imageBuffer;
  int size;
  int quality;
  int w;
  int h;
  int spaceColor;
  struct treeNode *next; 
} treeNode;

// fileName arriva dall'inserimento
struct treeNode *createNode(char *fileName, int w, int h, int quality, int colorSpace){

    treeNode *head = malloc(sizeof(treeNode));
    head->w = w;
    head->h = h;
    head->quality = quality;
    head->spaceColor = colorSpace;
    // Popolo il nodo con le informazioni che arrivano dalla funzione
    getBlob(fileName, w, h, quality, colorSpace, head->size);
    return head;
}

// Devo passare la testa della coda in modo da aggiungere il nodo alla fine
void addNode(){
    // Aggiungo un nodo alla fine della lista
}




