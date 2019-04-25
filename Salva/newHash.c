#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct hashNode {
   long key;
   char *imageBuffer;
   struct hashNode *next;
   int imageSize;
};

typedef struct hashNode hashNode;
#define SIZE 2
hashNode *testa[SIZE] = { NULL }; 

long hashCode(long key) {
   // Questa funzione prende in input un numero e ne restituisce il modulo
   // relativo alla dimensione della tabella Hash impostata
   return key % SIZE;
}

signed long hash(char *str){
    signed long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash * 33) + hash) + c;
    return hash;
}

int *deleteNode(long test){
    int hashIndex = hashCode(test);
    printf("\nHead is: %x and next is: %x\n", testa[hashIndex], testa[hashIndex]->next);
    if (testa[hashIndex]->key == test && testa[hashIndex]->next != NULL){
        printf("Entro nel primo?");
        hashNode *toDelete = testa[hashIndex];
        testa[hashIndex] = testa[hashIndex]->next;
        int size = toDelete->imageSize;
        free(toDelete);
        return size;
    }
    else if (testa[hashIndex]->key == test && testa[hashIndex]->next == NULL){
        printf("Entro nel secondo?");
        hashNode *toDelete = testa[hashIndex];
        int size = testa[hashIndex]->imageSize;
        free(testa[hashIndex]);
        testa[hashIndex] = NULL;
        return size;
    } else {
        hashNode *support = testa[hashIndex];
        while(support->next->key != test){
            support = support->next;
        }
        if (support->next->next != NULL){
            hashNode *toDelete = support->next;
            int size = toDelete->imageSize;
            free(toDelete);
            support->next = support->next->next;
            return size;
        } else {
            int size = support->next->imageSize;
            free(support->next);
            support->next = NULL;
            return size;
        }
    }
}

char* searchHashNode(char *string, int w, int h, int quality, int colorSpace){

    char *toHash = malloc(4*sizeof(int) + 4*sizeof(char)+strlen(string));
    sprintf(toHash, "%s|%d|%d|%d|%d", string, w, h, quality, colorSpace);
    
    long key = hash(toHash);
    int hashIndex = hashCode(key);
    printf("\nHash index is: %d\n", hashIndex);

    hashNode *support = testa[hashIndex];
    while(support != NULL){
        if (support->key == key){
            printf("\nFound\n");
            return;
        }
        support = support->next;
    }

    printf("\nNot found\n");

    hashNode *testNode = malloc(sizeof(hashNode));
    testNode->next = NULL;
    testNode->key = key;
    testNode->imageBuffer = NULL;

    insertHashNode(testNode, hashIndex);

    printf("Inserted");

    // Inserisco il nodo nella tabella Hash

}

void insertHashNode(hashNode *newNode, int index){    
    if (testa[index] == NULL){
        testa[index] = newNode;
        return;
    } else {
        hashNode *support = testa[index];
        while(support->next != NULL){
            support = support->next;
        }
        support->next = newNode;
        return;
    }
}

int main(void){

    searchHashNode("AAA", 10, 10, 10, 10);
    searchHashNode("AAA", 10, 10, 10, 10);
    searchHashNode("BBBB", 10, 10, 10, 10);
    searchHashNode("BBBB", 10, 10, 10, 10);
    deleteNode(4247311488675142450);
    searchHashNode("AAA", 10, 10, 10, 10);
}