#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>



int TestAndSet(int *old_ptr , int new){
    int old = *old_ptr ; 
    *old_ptr = new ; 
    return old;
}

typedef struct __lock_t{
    int flag ;
} 
lock_t;


void init(lock_t *mutex){
    mutex->flag = 0;
}

void lock(lock_t *mutex){
    while(mutex->flag == 1){
        // do nothing 
    }
    thrd_yield();
    mutex->flag = 1 ; 
}
void unlock(lock_t *mutex){
    mutex->flag = 0 ; 
}


int main(int argc , char* argv[]){
    //lock_t mutex ; 
    //lock(&mutex)
    
        
    return 0;
}