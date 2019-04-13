#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nodo{
  char indice[128];
  struct nodo* suc;
} nodo;

struct nodo* coda;

struct nodo* libera_n(struct nodo *testa,struct nodo *n){
  //nodo e' il puntatore al nodo da liberare
  //restituisce il puntatore alla testa
   struct nodo* new;
   struct nodo* aus;
   aus=testa;
   if(testa->indice==n->indice){
      new=n->suc;
      free(n->indice);
      free(n);
      return new;
   }
   else{
      
     while(testa->suc->indice!=n->indice){
        if(testa->suc->suc==NULL){
          puts("ERRORE:nodo non presente nella lista");
          return aus;
        }
        testa=testa->suc;
     }
     testa->suc=n->suc;
     free(n->indice);
     free(n);
     return aus;
   }
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
  if(testa==NULL){
    testa=nod;
    coda=nod;
    return testa;
  }
  while(testa!=NULL){
    aus=testa->suc;
    if(strcmp(testa->indice,nod->indice)==0){
      if(testa->suc!=NULL){
        
          if(testa==aus2){
              aus2=testa->suc;
          }else{
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

void main(){
    struct nodo* testa;
    testa=NULL;
    coda=NULL;
    
    // Ho il solo ind1

    struct nodo *n=malloc(sizeof(struct nodo));
    if(!n){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    strcpy(n->indice,"ind1");
    n->suc=NULL;
    testa=inserisci_n(testa,n);

    stampa(testa);
    
    // Inserisco ind2

    struct nodo *n2=malloc(sizeof(struct nodo));
    if(!n2){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    strcpy(n2->indice,"ind2");
    n2->suc=NULL;
    testa=inserisci_n(testa,n2);
    
    stampa(testa);
    
    // Inserisco ind3

    struct nodo *n3=malloc(sizeof(struct nodo));
    if(!n3){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    strcpy(n3->indice,"ind3");
    n3->suc=NULL;
    testa=inserisci_n(testa,n3);
  
    stampa(testa);

    // Ou

    struct nodo *n4=malloc(sizeof(struct nodo));
    if(!n4){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    strcpy(n4->indice,"ind4");
    n4->suc=NULL;
    testa=inserisci_n(testa,n4);

    stampa(testa);

    struct nodo *n5=malloc(sizeof(struct nodo));
    if(!n5){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    strcpy(n5->indice,"ind5");
    n5->suc=NULL;
    testa=inserisci_n(testa,n5);

    stampa(testa);

    struct nodo *n6=malloc(sizeof(struct nodo)); //errore:la malloc restituisce lo stesso puntatore del nodo4. Perche'?
    if(!n6){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    strcpy(n6->indice,"ind6");
    n6->suc=NULL;
    testa=inserisci_n(testa,n6);
    
    stampa(testa); 

    testa=inserisci_n(testa,n);
    stampa(testa); 
    
    testa=inserisci_n(testa,n4);
    stampa(testa); 
    
    testa=libera_n(testa,n);
    stampa(testa);
    printf("\nASD2:%s\n",n->indice);//perche' non da errore??
    testa=libera_n(testa,testa);    
    stampa(testa);
    testa=libera_n(testa,n);    
    stampa(testa);
    
}
