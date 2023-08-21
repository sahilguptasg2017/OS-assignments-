#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char *argv[]){
    printf("Parent (P) is having ID %d\n",getpid());
    int parent_pid = getpid();
    int rc = fork();
    if(rc<0){
        printf("No child being created\n");
        exit(1);
    }
    else if(rc==0){
        printf("Child is having ID %d\n",getpid());
        printf("My Parent ID is %d\n",parent_pid);
    }
    else{
        int wc = wait(NULL);
        printf("ID of P's Child is %d\n",rc);
    }   
    return 0;
}