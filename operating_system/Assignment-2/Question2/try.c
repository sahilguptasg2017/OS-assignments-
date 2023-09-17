#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

void print_timestamp(const char* label) {
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("%s: %ld.%09ld\n", label, timestamp.tv_sec, timestamp.tv_nsec);
}
int priority_other;
int priority_rr ;    
int priority_fifo;  
struct timespec start[3], end[3], time_for_scheduling[3];

int main(int argc, char *argv[]) {
    priority_other = 0;
    priority_rr = 5;
    priority_fifo = 5;
    int status = 0;
    int rc[3];
    for (int i = 0; i < 3; i++) {
        clock_gettime(CLOCK_REALTIME, &start[i]);
        rc[i] = fork();
        if (rc[i] < 0) {
            perror("fork failed");
            exit(1);
        } 
        else if (rc[i] == 0) {
            if (i == 0) {
                printf("child 1 started\n");
                struct sched_param parameter_A;
                parameter_A.sched_priority = priority_other;
                sched_setscheduler(0, SCHED_OTHER, &parameter_A);
                execl("./count", NULL);
                perror("execl failed");
            } 
            else if (i == 1) {
                struct sched_param parameter_B;
                parameter_B.sched_priority = priority_rr;
                sched_setscheduler(0, SCHED_RR, &parameter_B);
                printf("child 2 started\n");
                execl("./count", NULL);
                perror("execl failed");
            }
            else if (i == 2) {
                struct sched_param parameter_C;
                parameter_C.sched_priority = priority_fifo;
                sched_setscheduler(0, SCHED_FIFO, &parameter_C);
                printf("child 3 started\n");
                execl("./count", NULL);
                perror("execl failed");
            }            
          //  struct timespec child_start;
        //    clock_gettime(CLOCK_REALTIME, &child_start);
            //printf("Child number:%d - ", i);
            //print_timestamp("Child started");
            exit(1);
    }

    }   
    for (int i = 0; i<3;i++) {
        waitpid(rc[i], &status, 0); 
        clock_gettime(CLOCK_REALTIME, &end[i]);
    }

    for (int i = 0;i<3;i++) {
        struct timespec duration = {
            end[i].tv_sec - start[i].tv_sec,
            end[i].tv_nsec - start[i].tv_nsec
        };
        if(duration.tv_nsec < 0) {
            duration.tv_sec -= 1;
            duration.tv_nsec += 1000000000; 
        }
        if(i == 0) {
            printf("SCHED_OTHER took time: %ld.%09ld\n", duration.tv_sec, duration.tv_nsec);
            
        } 
        else if (i == 1) {
            printf("SCHED_RR took time: %ld.%09ld\n", duration.tv_sec, duration.tv_nsec);
        } 
        else if (i == 2) {
            printf("SCHED_FIFO took time: %ld.%09ld\n", duration.tv_sec, duration.tv_nsec);
        }
        time_for_scheduling[i] = duration;
    }
    FILE *ptr;
    ptr = fopen("time.txt", "a");
    if (ptr == NULL) {
        perror("fopen failed");
        exit(1);
    }
    fprintf(ptr, "%d ",priority_other);
    
    fprintf(ptr, "%d ",priority_rr);

    fprintf(ptr, "%d ",priority_fifo);
    for (int i = 0; i < 3; i++) {
        fprintf(ptr, "%ld.%09ld ", time_for_scheduling[i].tv_sec, time_for_scheduling[i].tv_nsec);
    }
    fprintf(ptr, "\n");
    fclose(ptr);
    char *args[] = {"python3", "scheduler.py", NULL};
    execvp("python3", args);
    perror("execvp failed");
    exit(1);
    return 0;
}
    