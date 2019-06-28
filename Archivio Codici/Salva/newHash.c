#include <stdio.h>
#include <stdbool.h>
#include "imageToBlob.h"
#include "newHash.h"
#include "coda.h"

nodo *testaCoda;
hashNode *testaHash[SIZE] = { NULL }; 

long hashCode(long key) {
   return key % SIZE;
}

signed long hash(char *str){
    signed long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash * 33) + hash) + c;
    return hash;
}

int deleteHashNode(long test){

    int hashIndex = hashCode(test);
    int size;

    if (testaHash[hashIndex]->key == test && testaHash[hashIndex]->next != NULL){

        printf("Entro nel primo?");
        hashNode *toDelete = testaHash[hashIndex];
        testaHash[hashIndex] = testaHash[hashIndex]->next;
        size = toDelete->imageSize;
        MagickRelinquishMemory(toDelete->imageBuffer);
        free(toDelete);
        return size;

    } else if (testaHash[hashIndex]->key == test && testaHash[hashIndex]->next == NULL){

        printf("Entro nel secondo?");
        hashNode *toDelete = testaHash[hashIndex];
        size = toDelete->imageSize;
        MagickRelinquishMemory(toDelete->imageBuffer);
        free(toDelete);
        testaHash[hashIndex] = NULL;
        return size;

    } else {

        hashNode *support = testaHash[hashIndex];
        while(support->next->key != test){
            support = support->next;
        }

        if (support->next->next != NULL){

            hashNode *toDelete = support->next;
            size = toDelete->imageSize;
            MagickRelinquishMemory(toDelete->imageBuffer);  
            free(toDelete);
            support->next = support->next->next;
            return size;

        } else {

            size = support->next->imageSize;
            MagickRelinquishMemory(support->imageBuffer);
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

    hashNode *support = testaHash[hashIndex];
    while(support != NULL){

        if (support->key == key){

            printf("\nFound\n");
            return support->imageBuffer;
        }

        support = support->next;
    }

    printf("\nNot found\n");

    hashNode *testNode = malloc(sizeof(hashNode));
    testNode->next = NULL;
    testNode->key = key;
    testNode->imageBuffer = getBlob(string, w, h, quality, colorSpace, &(testNode->imageSize));
    limite_dimensione-=testNode->imageSize;

    while( (limite_dimensione - testNode->imageSize) < 0 ){
        libera_ln(testaCoda);
    }

    insertHashNode(testNode, hashIndex);

}

void insertHashNode(hashNode *newNode, int index){

    // Effettuo l'inserimento nella coda con priorità
    testaCoda = inserisci_n(testaCoda, newNode->key);

    if (testaHash[index] == NULL){

        testaHash[index] = newNode;
        return;

    } else {

        hashNode *support = testaHash[index];

        while(support->next != NULL){
            support = support->next;
        }
        
        support->next = newNode;
        return;
    }
}

int main(void){

    MagickWandGenesis();

    testaCoda = NULL;

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("AAA", 10, 10, 10, 10);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("AAA", 10, 10, 10, 10);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("BBBB", 10, 10, 10, 10);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    searchHashNode("BBBB", 10, 10, 10, 10);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    printf("\nLa dimensione eliminata è: %d\n", deleteHashNode(4247311488675142450));
    searchHashNode("AAA", 10, 10, 10, 10);

    printf("\nAvaliable dimension is: %d\n", limite_dimensione);
    stampa(testaCoda);

    MagickWandTerminus();
}