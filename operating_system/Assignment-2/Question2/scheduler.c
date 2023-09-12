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


void print_timestamp(const char* label) {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    printf("%s: %ld.%09ld\n", label, timestamp.tv_sec, timestamp.tv_nsec);
}

struct timespec min(struct timespec a,struct timespec b){
    long min1 , min2;
    min1 = (long)(a.tv_sec)/1000 + a.tv_nsec*1000000;
    min2 = (long)(b.tv_sec)/1000 + b.tv_nsec*1000000;
    if(min1>min2){
        return a;
    }
    else{
        return b;
    }
}

int main() {
    struct timespec start1, start2, start3, end1, end2, end3;
    clock_gettime(CLOCK_MONOTONIC, &start1);
    int rc1 = fork();
    if (rc1 < 0) {
        printf("fork failed");
        exit(1);
    }
    else if (rc1 == 0) {
        print_timestamp("child 1 started");
        struct sched_param parameter;
        nice(0);
        sched_setscheduler(0, SCHED_OTHER, &parameter);
        sched_yield();
        execl("./count", "./count", NULL);
        printf("child 1 finished\n");
    } else if (rc1 > 0) {
        clock_gettime(CLOCK_MONOTONIC, &start2);
        int rc2 = fork();
        if (rc2 < 0) {
            perror("fork failed");
            exit(1);
        }
        else if (rc2 == 0) {
            print_timestamp("child 2 started\n");
            struct sched_param parameter;
            parameter.sched_priority=50;
            sched_setscheduler(0, SCHED_RR, &parameter);
            sched_yield();
            execl("./count", "./count", NULL);
            printf("child 2 finished\n");
        } 
        else if (rc2 > 0) {
            clock_gettime(CLOCK_MONOTONIC, &start3);
            int rc3 = fork();
            if (rc3 < 0) {
                perror("fork failed");
                exit(1);
            } 
            else if (rc3 == 0) {
                print_timestamp("child 3 started\n");
                struct sched_param parameter;
                parameter.sched_priority = 99 ;
                sched_setscheduler(0, SCHED_FIFO, &parameter);
                sched_yield();
                execl("./count", "./count", NULL);
                printf("child 3 finished\n");
            }   
            else if (rc3 > 0) {
                int status;
                waitpid(rc1,&status,0);
                clock_gettime(CLOCK_MONOTONIC, &end1);
                waitpid(rc2,&status,0);
                clock_gettime(CLOCK_MONOTONIC, &end2);
                waitpid(rc3, &status, 0);
                clock_gettime(CLOCK_MONOTONIC, &end3);
                struct timespec start_time_sec = min(min(start1,start2),start3);
                long sched_other_time_sec = end1.tv_sec - start_time_sec.tv_sec;
                long sched_other_time_nsec = end1.tv_nsec - start_time_sec.tv_nsec;
                if (sched_other_time_nsec < 0) {
                    sched_other_time_sec -= 1;
                    sched_other_time_nsec += 1000000000;
                }
                long sched_rr_time_sec = end2.tv_sec - start_time_sec.tv_sec;
                long sched_rr_time_nsec = end2.tv_nsec - start_time_sec.tv_nsec;
                if (sched_rr_time_nsec < 0) {
                    sched_rr_time_sec -= 1;
                    sched_rr_time_nsec += 1000000000;
                }
                long sched_fifo_time_sec = end3.tv_sec - start_time_sec.tv_sec;
                long sched_fifo_time_nsec = end3.tv_nsec - start_time_sec.tv_nsec;
                if (sched_fifo_time_nsec < 0) {
                    sched_fifo_time_sec -= 1;
                    sched_fifo_time_nsec += 1000000000;
                }
                printf("sched_other took time: %ld.%ld\n", sched_other_time_sec, sched_other_time_nsec);
                printf("sched_rr took time: %ld.%ld\n", sched_rr_time_sec, sched_rr_time_nsec);
                printf("sched_fifo took time: %ld.%ld\n", sched_fifo_time_sec, sched_fifo_time_nsec);
                FILE *ptr;
                ptr = fopen("time.txt", "w");
                char str[20];
                snprintf(str,sizeof(str),"%ld.%ld",sched_other_time_sec,sched_other_time_nsec);
                fputs(str,ptr);
                fputs("\n",ptr);
                snprintf(str,sizeof(str),"%ld.%ld",sched_rr_time_sec,sched_rr_time_nsec);
                fputs(str,ptr);
                fputs("\n", ptr);
                snprintf(str, sizeof(str), "%ld.%ld", sched_fifo_time_sec,sched_fifo_time_nsec);
                fputs(str, ptr);
                fputs("\n", ptr);

                fclose(ptr);            
                char *args[] = {"python3","scheduler.py",NULL};
                execvp("python3", args);
            }
        }
    }
    return 0;
}