#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int a=0;
int b=1;
void printfibon(int count){
    int c=0;
    if(count>0){
        c=a+b;
        a=b;
        b=c;
        printf(" %d",c);
        printfibon(count-1);
    }
}
int main(int argc,char *argv[]){
    int rc=vfork();
    if(rc<0){
        printf("Fork is failed");
        exit(1);
    }
    else if(rc==0){
        int child_pid=getpid();
        int n=1;
        for(int i=1;i<=4;i++){
            n*=i;
        }
        sleep(1);
        char* path="/home/sahilg/OS/OS-assignments-/operating_system/Assignment-1/Question1/Question1-bonus";
        char* args[]={path,NULL};
        char* env[]={NULL};
        execve(path,args,env);
        _exit(1);
        printf("Child is calculating , Factorial of 4 is: %d\n",n);
        
    }
    else{
        int count=16;           
        printf("Parent is calculating , Fibonnaci series up to 16: 0 1");
        printfibon(count-2);
        printf("\n");
        wait(NULL);
    }
    return 0;
}

