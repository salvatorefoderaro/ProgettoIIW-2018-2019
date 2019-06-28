#include <pthread.h>
#include <stdio.h>
pthread_rwlock_t semaforo; 
void* t1(){
 while(1){   
  pthread_rwlock_wrlock(&semaforo);
    puts("Sono 1:inserisci il carattere 1");
    int d;
    scanf("%d",&d);
    while(d!=1){
        printf("%d",d);
        puts("Sono 1:inserisci il carattere 1");
        scanf("%d",&d);
    }
  pthread_rwlock_unlock(&semaforo);
  sleep(1);
 }  
}
void* t2(){
  while(1){   
  pthread_rwlock_wrlock(&semaforo);
    puts("Sono 2:inserisci il carattere 2");
    int d;
    scanf("%d",&d);
    while(d!=2){
        printf("%d",d);
        puts("Sono 2:inserisci il carattere 2");
        scanf("%d",&d);
    }
  pthread_rwlock_unlock(&semaforo);
  sleep(10);
 }
}

void* t3(){
  while(1){   
  pthread_rwlock_rdlock(&semaforo);
  int d=1;
  while(d<20){ 
    printf("%d",d);
    puts("Sono 3:sto leggendo");
    sleep(1);
    d+=1;
  }  
  pthread_rwlock_unlock(&semaforo);
  sleep(10);
 }
}

void* t4(){
  while(1){   
  pthread_rwlock_rdlock(&semaforo);
  int d=1;
  while(d<10){ 
    printf("%d",d);
    puts("Sono 4:sto leggendo");
    sleep(1);
    d+=1;
  }  
  pthread_rwlock_unlock(&semaforo);
  sleep(10);
 }
}

void* t5(){
  while(1){   
  pthread_rwlock_rdlock(&semaforo);
  int d=1;
  while(d<25){ 
    printf("%d",d);
    puts("Sono 5:sto leggendo");
    sleep(1);
    d+=1;
  }  
  pthread_rwlock_unlock(&semaforo);
  sleep(10);
 }
}

void main(){
  pthread_t thread;
  pthread_create(&thread, NULL, t1, NULL);
  pthread_create(&thread, NULL, t2, NULL);

  pthread_create(&thread, NULL, t3, NULL);
  pthread_create(&thread, NULL, t4, NULL);
  pthread_create(&thread, NULL, t5, NULL);
   
  sleep(1000); 
}