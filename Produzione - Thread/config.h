#ifndef _CONFIG_H 
#define _CONFIG_H
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define dimensioneTotale 50000000 // Dimensione cache in byte
#define htdocsPath "htdocs" // Cartella contente le risorse web
#define flushTime 60 // Numero di secondi del flush su file
#define socketTimeout 5 // Numero di secondi per il timeout del socket
#define SIZE 50 // Numero di indici per la tabella Hash

#endif