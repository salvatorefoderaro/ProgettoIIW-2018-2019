#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTableCoda.h"
#include "codaHash.h"

struct nodo* coda;

struct nodo* libera_n(struct nodo *n){
  //nodo e' il puntatore al nodo da liberare
  //restituisce il puntatore al nuovo nodo della lista (e quindi alla nuova testa)
   struct nodo* new;
   new=n->suc;
   free(n->indice);
   free(n);
   return new;
}

void libera_ln(struct nodo *testa){
  //testa e' il puntatore alla radice della lista collegata
  //libera la lista dalla memoria
  struct nodo* aus;
  while(testa!=NULL){
    aus=testa->suc;
    free(testa->indice); 
    free(testa);
    testa=aus;
  }
}

struct nodo* inserisci_in_coda(struct nodo *coda,struct nodo *mes){
  //testa e' l' ultimo nodo della lista, mes il nodo da inserire
  //la funzione aggiorna il puntatore all' ultimo nodo con quello appena inserito
  if(coda==NULL){
    coda=mes;
  }else{
    coda->suc=mes;
    coda=mes;
    mes->suc=NULL;
  }
  return coda;
}

struct nodo* inserisci_n(struct nodo* testa,struct nodo* nod){
  struct nodo* aus;
  struct nodo* aus1;
  struct nodo* aus2;
  aus2=testa;
  aus1=aus2;
  while(testa!=NULL){
    aus=testa->suc;
    if(strcmp(testa->indice,nod->indice)==0){
      if(testa->suc!=NULL){
        
          if(testa==aus2){
              aus2=testa->suc;
              printf("\nCIAO:");
              puts(aus2->indice);
          }else{
              puts(aus1->indice);
              aus1->suc=testa->suc;
          }   
          break;
      }else{
          testa=aus2;
          return testa;
      }
    } 
    aus1=testa;
    testa=aus;
  }
  coda=inserisci_in_coda(coda,nod);  
  testa=aus2;
  return testa;
}

void stampa(struct nodo *testa){
  struct nodo* aus;
  while(testa!=NULL){
    aus=testa->suc;
    printf("STAMPA:%s\n",testa->indice);
    testa=aus;
  }
  printf("Coda:%s\n\n",coda->indice);
}

/* void main(){
    struct nodo* testa;
    testa=NULL;
    coda=NULL;
    
    // Ho il solo ind1

    struct nodo *n=malloc(sizeof(struct nodo));
    strcpy(n->indice,"ind1");
    n->suc=NULL;
    testa=n;
    coda=n;

    stampa(testa);
    
    // Inserisco ind2

    struct nodo *n2=malloc(sizeof(struct nodo));
    strcpy(n2->indice,"ind2");
    n2->suc=NULL;
    testa=inserisci_n(testa,n2);
    
    stampa(testa);
    
    // Inserisco ind3

    struct nodo *n3=malloc(sizeof(struct nodo));
    strcpy(n3->indice,"ind3");
    n3->suc=NULL;
    testa=inserisci_n(testa,n3);
  
    stampa(testa);

    // Ou

    struct nodo *n4=malloc(sizeof(struct nodo));
    strcpy(n4->indice,"ind4");
    n4->suc=NULL;
    testa=inserisci_n(testa,n4);

    stampa(testa);

    struct nodo *n5=malloc(sizeof(struct nodo));
    strcpy(n5->indice,"ind5");
    n5->suc=NULL;
    testa=inserisci_n(testa,n5);

    stampa(testa);

    struct nodo *n6=malloc(sizeof(struct nodo)); //errore:la malloc restituisce lo stesso puntatore del nodo4. Perche'?
    strcpy(n6->indice,"ind6");
    n6->suc=NULL;
    testa=inserisci_n(testa,n6);
    
    stampa(testa); 


    testa=inserisci_n(testa,n);
    stampa(testa); 
    
    testa=inserisci_n(testa,n4);
    stampa(testa); 



    printf("ASD:%s\n",n->indice);//perche' non da errore??
    libera_ln(testa);
    printf("%s\n",testa->indice);//perche' non da errore??
} */