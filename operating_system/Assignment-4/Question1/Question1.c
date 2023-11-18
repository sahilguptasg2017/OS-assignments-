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

void eating(int id){
    printf("Philospher %d is eating right now.\n",id);
    sleep(2) ;
}
 
void thinking(int id){
    printf("Philospher %d is thinking right now.\n",id);
    sleep(1) ;
} 


void* philospher(void* args){   
    int id = *(int*)args ; 
    int left_fork = left(id) ;
    int right_fork = right(id) ;
    while(1){
        thinking(id) ;

        pthread_mutex_lock(&forks[left_fork]) ;
        pthread_mutex_lock(&forks[right_fork]) ;
        pthread_mutex_lock(&bowl[0]) ;
        pthread_mutex_lock(&bowl[1]) ;

        eating(id) ; 
        
        pthread_mutex_unlock(&forks[left_fork]) ;
        pthread_mutex_unlock(&forks[right_fork]) ;
        pthread_mutex_unlock(&bowl[0]) ;
        pthread_mutex_unlock(&bowl[1]) ;
    
        pthread_cond_signal(&cond[id]) ;
    }
}




int main(){
    //thinking(2) ;
    pthread_t philosphers[5] ; // 5 threads for philoshers .. 
    int id[5] ;
    for(int i = 0 ;i<5;i++){
        pthread_mutex_init(&forks[i],NULL);
        pthread_cond_init(&cond[i],NULL) ;
    }
    for(int i = 0 ;i<2;i++){
        pthread_mutex_init(&bowl[i],NULL);
    }
    for(int i=0;i<5;i++){
        id[i] = i ;
        pthread_create(&philosphers[i],NULL,philospher,&id[i]); 
    }

    for(int i =0;i<5;i++){
        pthread_join(philosphers[i],NULL) ;
    }   

    for(int i =0;i<5;i++){
        pthread_mutex_destroy(&forks[i]);
        pthread_cond_destroy(&cond[i]);
    }
    
    for(int i =0 ;i<2;i++){
        pthread_mutex_destroy(&bowl[i]) ;
    }

    return 0 ;
}