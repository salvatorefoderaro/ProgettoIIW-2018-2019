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
#define backlog 256 // Numero massimo di connessioni in coda per la listen()
#define workSetDetection 10 // Numero di thread da utilizzare per il parsing dello user-agent
#define cacheItemDetection 1000 // Numero di parsing dello user-agent da memorizzare in cache

#endif