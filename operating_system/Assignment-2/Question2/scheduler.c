#include <bits/time.h>
#include <bits/types/struct_sched_param.h>
#include <linux/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
int main(){
    for(int i=0 ;i<3;i++){
        struct timespec start;
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC,&start);
        int rc =fork();
        if(rc<0){
            printf("fork failed");
            exit(1);
        }
        else if(rc == 0){
            if(i == 0){
                struct sched_param parameter;
                parameter.sched_priority = 1;
                sched_setscheduler(0, SCHED_OTHER,&parameter);
                execl("./count","./count",NULL);
            }
            else if(i == 1){
                struct sched_param parameter;
                parameter.sched_priority = 1;
                sched_setscheduler(0,SCHED_RR,&parameter);
                execl("./count","./count",NULL);
            }
            else{
                struct sched_param parameter;
                parameter.sched_priority = 1;
                sched_setscheduler(0, SCHED_FIFO,&parameter);  
                execl("./count","./count",NULL);
            }
        }   
        else if(rc >0){
            int status;
            int wc = waitpid(rc,&status,0);
            clock_gettime(CLOCK_MONOTONIC,&end); 
            long time_spent_seconds = end.tv_sec - start.tv_sec ; 
            long time_spent_nseconds = end.tv_nsec -start.tv_nsec;
            if(time_spent_nseconds < 0){
                time_spent_seconds-=1;
                time_spent_nseconds+=1000000000;
            }
            printf("%d %ld:%ld\n",i,time_spent_seconds,time_spent_nseconds);
        } 
    }
    return 0;
}