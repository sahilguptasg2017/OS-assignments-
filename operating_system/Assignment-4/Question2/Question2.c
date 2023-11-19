#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>



void* car(void* args){
        


}

void* passengers(void* args){



}

void load(){
    printf("Car is loading passengers ..\n");
    sleep(2) ;
}
void unload(){
    printf("Car is unloading passengers ..\n");
    sleep(2) ;
}
void board(int ID){
    printf("passenger with ID %d is boarding the car.",ID) ;

}
void offboard(int ID){
    printf("passenger with ID %d is offboarding the car.",ID) ;
}



int main(){
    int total_passengers ;
    printf("Select the total number of passengers: ") ;
    scanf("%d",&total_passengers);
    int capacity ; 
    printf("Select the capacity of the car: ") ;
    scanf("%d",&capacity) ;
    
    pthread_t passengers[total_passengers] ;
    pthread_t car[1] ;
    int id[total_passengers] ;

    pthread_create(&car[0], NULL, car, NULL);

    for(int i=0;i<total_passengers;i++){
        id[i] = i ;
        pthread_create(&passengers[i],NULL,passengers,&id[i]) ;
        
    }

    for(int i = 0;i<total_passengers ; i++){
        pthread_join(passengers[i], NULL) ;
    }
    

    return 0;
}