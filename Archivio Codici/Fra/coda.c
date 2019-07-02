#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coda.h"

struct nodo* coda; //puntatore alla coda della lista collegata

struct nodo* libera_n(struct nodo *testa,long n){
  //testa e' il puntatore alla testa della coda,n e' il puntatore al nodo da liberare
  //restituisce il puntatore alla testa
   struct nodo* new;
   struct nodo* aus;
   aus=testa;
   if(testa->indice==n){ //se il nodo da eliminare e' quello in testa 
      extern limite_dimensione;
      limite_dimensione-=deleteHash(n);//CONTROLLARE perche' la funzione puo' ritornare NULL
      new=testa->suc; //la nuova testa punta al successivo della vecchia 
      free(testa);
      return new; //ritorno la nuova testa
   }
   else{ //se il nodo da eliminare e' nel mezzo della lista
      
     while(!(testa->suc->indice==n)){ //cerco il nodo nella lista
     //testa tiene traccia del nodo precedente al nodo da eliminare
        if(testa->suc->suc==NULL){ //se il nodo testa->suc non e' quello cercato e il nodo successivo e' NULL => il nodo ricercato non e' nella lista
          puts("ERRORE:nodo non presente nella lista");
          return aus; //ritorno la testa(non e' stata modificata)
        }
        testa=testa->suc;//vado al nodo successivo per continuare la ricerca
     }
     //se ho trovato il nodo:
     testa->suc=testa->suc->suc; //il nodo precedente punta al nodo successivo 
     if(testa->suc==NULL){
       coda=testa;
     } 
     free(testa->suc);
     return aus; //ritorno la testa(non modificata)
   }
}

void libera_ln(struct nodo *testa){
  //testa e' il puntatore alla radice della lista collegata
  //libera la lista dalla memoria
  struct nodo* aus;
  while(testa!=NULL){
    aus=testa->suc;
    free(testa);
    testa=aus;
  }
}

struct nodo* inserisci_in_coda(struct nodo *coda,struct nodo *mes){//si potrebbe modificare in quanto coda e' globale(si potrebbe fare la funzione a void e non passare la coda come parametro)
  //coda e' l' ultimo nodo della lista, mes il nodo da inserire
  //la funzione aggiorna il puntatore all' ultimo nodo con quello appena inserito
  if(coda==NULL){//se la lista e' vuota, la coda e' uguale all' unico nodo
    coda=mes;
  }else{
    coda->suc=mes; //la vecchia coda punta alla nuova 
    coda=mes; //aggiorno la coda
    mes->suc=NULL; //il puntatore (della coda) al nuovo nodo deve essere NULL (nel caso ho reinserimento di un nodo precedente se non metto questa riga avro' un ciclo infinito in quanto il nodo reinserito in passato puntava ad un altro nodo)
  }
  return coda;
}

struct nodo* inserisci_n(struct nodo* testa,long nod){
  struct nodo* aus; //puntatore al nodo successivo della lista
  struct nodo* aus1; //puntatore al nodo precente a quello da ricercare/inserire
  struct nodo* aus2;//puntatore alla testa della lista
  aus2=testa;
  aus1=aus2; 
  if(testa==NULL){ //se la lista e' vuota
    struct nodo *n=malloc(sizeof(struct nodo)); 
    if(!n){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    n->indice=nod;
    n->suc=NULL;
    testa=n;
    coda=n;
    return testa;
  }
  //ricerco nella lista il nodo da inserire
  while(testa!=NULL){
    aus=testa->suc; //tengo traccia del nodo successivo in quanto testa verra' modificato
    if(testa->indice==nod){ //se gli indici dei due nodi corrispondono
      if(testa->suc!=NULL){ //e il nodo non si trova gia' in coda(non ha senso sostituirlo se e' gia' in coda)
          //aggiornamento
          if(testa==aus2){  //se il nodo trovato e' in testa=> la nuova testa puntera' al nodo successivo della vecchia
              aus2=testa->suc;
          }else{ //se il nodo e' in mezzo alla lista: il precedente punta al successivo
              aus1->suc=testa->suc;
          }   
        coda=inserisci_in_coda(coda,testa);
          testa=aus2; //aus2 tiene conto di tutte le modifiche eventuali della testa della lista
          return testa;
      }else{
          testa=aus2; //se il nodo corrispondente e' in coda allora restituisco la testa non modificata
          return testa;
      }
    } 
    aus1=testa; //aus equivale cosi' al nodo precedente nel prossimo ciclo
    testa=aus; //testa viene incrementato al nodo successivo della lista
  }
  struct nodo *n2=malloc(sizeof(struct nodo)); 
    if(!n2){
      puts("Impossibile allocare memoria");
      exit(-1);
    }
    n2->indice=nod;
    n2->suc=NULL;
    
  coda=inserisci_in_coda(coda,n2);  
  testa=aus2; //aus2 tiene conto di tutte le modifiche eventuali della testa della lista
  return testa;
}

void stampa(struct nodo *testa){
  //testa e' il puntatore alla lista da stampare
  struct nodo* aus;
  if(testa==NULL){
    puts("Lista vuota");
    return;
  }
  while(testa!=NULL){
    aus=testa->suc;
    printf("STAMPA:%d\n",testa->indice);
    testa=aus;
  }
  printf("Coda:%d\n\n",coda->indice);
}
/*
 void main(){
    struct nodo* testa;
    testa=NULL;
    coda=NULL;
    
    // Ho il solo ind1
    long n=1;
    testa=inserisci_n(testa,n);
    stampa(testa);
    
    n=2;
    testa=inserisci_n(testa,n);
    stampa(testa);
    
    n=3;
    testa=inserisci_n(testa,n);
    stampa(testa);
    
    n=4;
    testa=inserisci_n(testa,n);
    stampa(testa);
    
    n=5;
    testa=inserisci_n(testa,n);
    stampa(testa);
    
    
    testa=inserisci_n(testa,n);
    stampa(testa);

    testa=libera_n(testa,testa->indice);    
    stampa(testa);

    testa=libera_n(testa,n);    
    stampa(testa);
    
    testa=libera_n(testa,n);    
    
}
 */