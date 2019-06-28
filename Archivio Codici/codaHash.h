#ifndef _CODAHASH_H 
#define _CODAHASH_H

struct nodo;

struct nodo{
  char indice[128];
  struct nodo* suc;
  struct DataItem *hashItem;
};

typedef struct nodo nodo;
struct nodo *coda;

struct nodo* libera_n(struct nodo *n);
void libera_ln(struct nodo *testa);
struct nodo* inserisci_in_coda(struct nodo *coda,struct nodo *mes);
struct nodo* inserisci_n(struct nodo* testa,struct nodo* nod);
void stampa(struct nodo *testa);

#endif 