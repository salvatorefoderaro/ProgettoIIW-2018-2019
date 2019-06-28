#ifndef _CODA_H 
#define _CODA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct nodo;

struct nodo{
  long indice;
  struct nodo* suc;
  struct hashNode *hashItem;
};

typedef struct nodo nodo;

extern int limite_dimensione;  /* Declaration of the variable */
extern const int dimensioneTotale;

struct nodo* coda;

struct nodo* libera_n(struct nodo *testa,struct nodo * n);
void libera_ln(struct nodo *testa);
struct nodo* inserisci_in_coda(struct nodo *coda,struct nodo *mes);
struct nodo* inserisci_n(struct nodo* testa,long nod, struct hashNode *h);
void stampa(struct nodo *testa);

#endif 