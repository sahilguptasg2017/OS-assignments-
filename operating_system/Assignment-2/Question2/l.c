#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>

pid_t pids[3];
struct timespec begin[3];
struct timespec end[3];
float time_val[3];

int OTHER_PRIORITY;
int RR_PRIORITY;
int FIFO_PRIORITY;

struct sched_param param1;
struct sched_param param2;
struct sched_param param3;


void setPid(int idx, pid_t pid){
    pids[idx] = pid;
}

void setBegin(int idx, struct timespec start_time){
    begin[idx] = start_time;
}

void setEnd(pid_t pid, struct timespec end_time){
    for(int i = 0; i < 3; i++) if(pids[i] == pid) end[i] = end_time;
}

int main(int argc, char* argv[]){

    OTHER_PRIORITY = atoi(argv[1]);
    RR_PRIORITY = atoi(argv[2]);
    FIFO_PRIORITY = atoi(argv[3]);

    pid_t child_pid, wpid;
    int status = 0;

    for(int i = 0; i < 3; i++){
        struct timespec curr_time; 
        clock_gettime(CLOCK_REALTIME, &curr_time); 
        setBegin(i, curr_time);
        if((child_pid = fork()) == 0){
            if(i == 0) {param1.sched_priority = OTHER_PRIORITY; sched_setscheduler(child_pid, SCHED_OTHER, &param1); perror("OTHER"); execvp("./count", NULL);}
            else if(i == 1) {param2.sched_priority = RR_PRIORITY; sched_setscheduler(child_pid, SCHED_RR, &param2); perror("RR"); execvp("./count", NULL);}
            else if(i == 2) {param3.sched_priority = FIFO_PRIORITY; sched_setscheduler(child_pid, SCHED_FIFO, &param3); perror("FIFO"); execvp("./count", NULL);}
            exit(0);
        }else setPid(i, child_pid);
    }
    
    while((wpid = wait(&status)) > 0) {struct timespec curr_time; clock_gettime(CLOCK_REALTIME, &curr_time); setEnd(wpid, curr_time);}

    time_val[0] = (end[0].tv_sec - begin[0].tv_sec) + ((float)end[0].tv_nsec - begin[0].tv_nsec)/1000000000;
    time_val[1] = (end[1].tv_sec - begin[1].tv_sec) + ((float)end[1].tv_nsec - begin[1].tv_nsec)/1000000000;
    time_val[2] = (end[2].tv_sec - begin[2].tv_sec) + ((float)end[2].tv_nsec - begin[2].tv_nsec)/1000000000;

    // printf("%lds %ldms\n", begin[0].tv_sec, begin[0].tv_nsec);
    // printf("%lds %ldms\n", begin[1].tv_sec, begin[1].tv_nsec);
    // printf("%lds %ldms\n", begin[2].tv_sec, begin[2].tv_nsec);
    // printf("%lds %ldms\n", end[0].tv_sec, end[0].tv_nsec);
    // printf("%lds %ldms\n", end[1].tv_sec, end[1].tv_nsec);
    // printf("%lds %ldms\n", end[2].tv_sec, end[2].tv_nsec);
    // printf("%f s\n", time_val[0]);
    // printf("%f s\n", time_val[1]);
    // printf("%f s\n", time_val[2]);
    // execvp(CLEAN_A, NULL)
    // execvp(CLEAN_B, NULL)
    // execvp(CLEAN_C, NULL)

    FILE* fout = fopen("time.txt", "a+");
    fprintf(fout, "%d %d %d %f %f %f\n", OTHER_PRIORITY, RR_PRIORITY, FIFO_PRIORITY, time_val[0], time_val[1], time_val[2]);
    fclose(fout);
    return 0;
}