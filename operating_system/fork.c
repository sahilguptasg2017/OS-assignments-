//  GNU nano 7.2   

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[]){
    printf("pid:%d\n",(int) getpid());
    int rc=fork();
    if(rc<0){
        fprintf(stderr,"fork failed\n"); //fork failed
        exit(1);
    }   
    else if(rc==0){
        printf("I am child (pid:%d\n)",(int) getpid()); //child 
    }
    else{
        printf("I am parent of %d (pid:%d\n)",rc,(int) getpid()); //parent 
        
    }
    return 0;
}