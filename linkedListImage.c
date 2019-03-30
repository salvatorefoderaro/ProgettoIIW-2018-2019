#include <stdlib.h>
#include <stdio.h>
#include "imageToBlob.c"

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

    treeNode *head1 = malloc(sizeof(treeNode));
    head1->w = w;
    head1->h = h;
    head1->quality = quality;
    head1->spaceColor = colorSpace;
    head1->next = NULL;

    // Popolo il nodo con le informazioni che arrivano dalla funzione
    head1->imageBuffer = getBlob(fileName, w, h, quality, colorSpace, &(head1->size));
    return head1;
}

char *searchNodeInList(treeNode *head1, char *fileName, int w, int h, int quality, int colorSpace, int* size){
    
    if (head1->h == h && head1->w == w){
        printf("\n     *****     Versione già presente in lista!     *****     ");
        return head1->imageBuffer;
    }

    // Controllo se il nodo è già presente nella lista
    while (head1->next != NULL){
        if (head1->h == h && head1->w == w){
            printf("\n      *****     Versione già presente in lista!     *****     ");
            return head1->imageBuffer;
        }
        head1 = head1->next;
    }
    
    printf("\n     *****     Versione non presente in lista! Procedo con l'inserimento!     *****     \n");
    // Se non ho trovato la versione che mi serve, la vado ad inserire
    treeNode *new = createNode(fileName, w, h, quality, colorSpace);
    head1->next = new;
    size = &(head1->next->size);
    return head1->next->imageBuffer;
}