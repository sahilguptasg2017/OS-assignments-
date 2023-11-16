#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

pthread_mutex_t forks[5] ;
pthread_mutex_t bowl[2] ;
pthread_cond_t cond[5] ;

int left(int p){
    return p;
}

int right ( int p ){
    return (p+1) % 5 ;
}

void* philospher(void* args){   
    
}


void eating(int id){
    printf("Philospher %d is eating right now.\n",id);
    sleep(2) ;
}

void thinking(int id){
    printf("Philospher %d is thinking right now.\n",id);
    sleep(1) ;
} 

int main(){
    thinking(2) ;




    return 0 ;
}