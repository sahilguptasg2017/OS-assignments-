#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> 

int total_passengers ;
int capacity ;




sem_t car_semaphore ;

sem_t lock  ;
 
sem_t loading_check  ;

sem_t unloading_check ;

sem_t capacity_check ; 


int current_capacity = 0; 

int current_threads = 0 ;


void load(){
    while(current_threads < total_passengers){

    }
    printf("car is loading ..\n") ;
    sleep(1) ;
}

void unload(){

    printf("car is unloading ..\n" ) ;
    sleep(1) ;
}
void* car(void* args){
    while(1){
        load() ;
        sem_post(&loading_check) ;
        while(current_capacity != capacity) {
           // printf("passengers are boarding ..") ;
        }
        sem_wait(&loading_check) ;
        sem_wait(&capacity_check) ;
        printf("Car is running ..\n") ;
        sleep(1) ;
        unload() ;
        sem_post(&unloading_check) ;

        while(current_capacity != 0 ){

            //printf("passengers are unboarding ..") ;

        }

        sem_post(&lock) ;

        for(int i = 0 ;i< capacity ;i++) {

            sem_post(&car_semaphore) ;



        }

        sem_wait(&lock) ;

        sem_wait(&unloading_check) ;

        sem_post(&capacity_check) ;




    }


    pthread_exit(NULL) ;

}

void board(int id){
    sem_wait(&lock) ;

    printf("passenger %d is curently boarding the car..\n",id) ;
    sleep(1) ;
    current_capacity += 1 ;
    
    sem_post(&lock) ;

}

void offboard(int id){
    sem_wait(&lock) ;

    printf("passenger %d is curently unboarding the car..\n",id) ;
    sleep(1) ;
    current_capacity -= 1 ;
    
    sem_post(&lock) ;


}

void* passengers(void* args){
    while(1){

        current_threads += 1 ;
        sem_wait(&car_semaphore) ;

        sem_wait(&loading_check) ;

        board(*(int*)args) ;

    
        sem_post(&loading_check) ;

        sem_wait(&unloading_check) ;

        offboard(*(int*) args) ;

        sem_post(&unloading_check) ;

    }

    pthread_exit(NULL) ;

}



int main (){
    
    printf("Give input for total number of passengers: ") ;
    scanf("%d",&total_passengers) ;
    printf("Give input for capacity of car: ") ;
    scanf("%d",&capacity) ;
    
    if(total_passengers<0 || capacity <0 || total_passengers < capacity){
        printf("this is a wrong input\n") ;
    }
    else{

        //making semaphore for car .. 
        sem_init(&car_semaphore , 0 , capacity) ;
        //making semahore for implementing lock .. 
        sem_init(&lock , 0 , 1) ;
        //making semaphore to check loading process .. 
        sem_init( &loading_check,0 , 0) ; 
        //making semaphore to check unloading process .. 
        sem_init(&unloading_check , 0 , 0) ;
        //making semaphore to check capacity .. 
        sem_init( &capacity_check , 0  , 1);
        

        pthread_t  my_passenger[total_passengers] ;

        int id[total_passengers] ;

        for(int i = 0;i<total_passengers ;i++){

            id[i] = i ;
            pthread_create(&my_passenger[i] , NULL , passengers , &id[i]) ;
        
        }        
        
        pthread_t my_car ;

        pthread_create(&my_car,NULL, car ,NULL) ;

        for(int i = 0 ;i<total_passengers ;i++){
    
            pthread_join(my_passenger[i],NULL) ;
     
        }

        pthread_join(my_car , NULL) ;


        sem_destroy(&car_semaphore) ;
        //destroying semaphore for car .. 
        sem_destroy(&lock) ;
        //destroying semaphore for implementing lock .. 
        sem_destroy(&loading_check) ;
        //destroying semaphore for implementing loding check .. 
        sem_destroy(&unloading_check) ;
        //destroying semaphore for implementing unloading check .. 
        sem_destroy(&capacity_check) ;
        //destroying semaphore for implementing capacity .. 
        
    }

    return 0 ;
}

