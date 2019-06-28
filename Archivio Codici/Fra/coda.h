#ifndef _CODA_H 
#define _CODA_H

struct nodo;

struct nodo{
  char indice;
  struct nodo* suc;
struct DataItem *hashItem;
};

typedef struct nodo nodo;

struct nodo* coda;

struct nodo* libera_n(struct nodo *testa,long n);
void libera_ln(struct nodo *testa);
struct nodo* inserisci_in_coda(struct nodo *coda,struct nodo *mes);
struct nodo* inserisci_n(struct nodo* testa,long nod);
void stampa(struct nodo *testa);

#endif 