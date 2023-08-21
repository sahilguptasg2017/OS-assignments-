#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include "common.ver"
#include <assert.h>
int main(int argc,char *argv[]){
    int *p=malloc(sizeof(int));  //a1
    assert(p != NULL);
    printf("(%d) address pointed to by p: %p\n",
        getpid(),p); //a2
        *p=0;       //a3
        while(1){
            *p+=1;
            printf("(%d) p: %d\n", getpid(),*p); //a4
        }
    return 0;
    
}

/*
it allocates some memory  @a1

then it print outs all the address of the memory @a2

put no 0 on the first slot of newly allocated array  @a3

finally it loops delaying for a second and ++ the value stored in p

 PID (process identifier )

*/