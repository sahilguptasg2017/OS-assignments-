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

struct timespec min(struct timespec a, struct timespec b) {
    long min1, min2;
    min1 = (long)(a.tv_sec) * 1000000000 + a.tv_nsec;
    min2 = (long)(b.tv_sec) * 1000000000 + b.tv_nsec;
    if (min1 > min2) {
        return b;
    } else {
        return a;
    }
}
struct sched_param parameter_A;
struct sched_param parameter_B;
struct sched_param parameter_C;
int priority_other;
int priority_rr;
int priority_fifo;

int main() {
    struct timespec start1, start2, start3, end1, end2, end3;
    priority_rr = 5;
    priority_fifo = 90;
    clock_gettime(CLOCK_MONOTONIC, &start1);
    int rc1 = fork();
    if (rc1 < 0) {
        perror("fork failed");
        exit(1);
    } 
    else if (rc1 == 0) {
        print_timestamp("child 1 started");
        parameter_A.sched_priority = priority_other;
        if (sched_setscheduler(0, SCHED_OTHER, &parameter_A) == -1) {
            perror("sched_setscheduler failed");
            exit(1);
        }
        execl("./count", "./count", NULL);
        perror("child 1 execl failed");
        exit(1);
    }
    else if (rc1 > 0) {
        clock_gettime(CLOCK_MONOTONIC, &start2);
        int rc2 = fork();
        if (rc2 < 0) {
            perror("fork failed");
            exit(1);
        }
        else if (rc2 == 0) {
            print_timestamp("child 2 started");
            parameter_B.sched_priority = priority_rr;
            if (sched_setscheduler(0, SCHED_RR, &parameter_B) == -1) {
                perror("sched_setscheduler failed");
                exit(1);
            }
            execl("./count", "./count", NULL);
            perror("child 2 execl failed");
            exit(1);
        }
        else if (rc2 > 0) {
            clock_gettime(CLOCK_MONOTONIC, &start3);
            int rc3 = fork();
            if (rc3 < 0) {
                perror("fork failed");
                exit(1);
            }
            else if (rc3 == 0) {
                print_timestamp("child 3 started");
                parameter_C.sched_priority = priority_fifo;
                if (sched_setscheduler(0, SCHED_FIFO, &parameter_C) == -1) {
                    perror("sched_setscheduler failed");
                    exit(1);
                }
                //sched_yield();
                execl("./count", "./count", NULL);
                perror("child 3 execl failed");
                exit(1);
            }
            else if (rc3 > 0) {
                int status;
                waitpid(rc1, &status, 0);
                clock_gettime(CLOCK_MONOTONIC, &end1);
                waitpid(rc2, &status, 0);
                clock_gettime(CLOCK_MONOTONIC, &end2);
                waitpid(rc3, &status, 0);
                clock_gettime(CLOCK_MONOTONIC, &end3);
                //struct timespec start_time_sec = min(min(start1, start2), start3);
                //struct timespec end_time_sec = min(min(end1, end2), end3);
                struct timespec sched_other_duration = {end1.tv_sec - start1.tv_sec, end1.tv_nsec - start1.tv_nsec};
                struct timespec sched_rr_duration = {end2.tv_sec - start2.tv_sec, end2.tv_nsec - start2.tv_nsec};
                struct timespec sched_fifo_duration = {end3.tv_sec - start3.tv_sec, end3.tv_nsec - start3.tv_nsec};
                if (sched_other_duration.tv_nsec < 0) {
                    sched_other_duration.tv_sec -= 1;
                    sched_other_duration.tv_nsec += 1000000000;
                }
                if (sched_rr_duration.tv_nsec < 0) {
                    sched_rr_duration.tv_sec -= 1;
                    sched_rr_duration.tv_nsec += 1000000000;
                }
                if (sched_fifo_duration.tv_nsec < 0) {
                    sched_fifo_duration.tv_sec -= 1;
                    sched_fifo_duration.tv_nsec += 1000000000;
                }
                printf("sched_other took time: %ld.%09ld\n", sched_other_duration.tv_sec, sched_other_duration.tv_nsec);
                printf("sched_rr took time: %ld.%09ld\n", sched_rr_duration.tv_sec, sched_rr_duration.tv_nsec);
                printf("sched_fifo took time: %ld.%09ld\n", sched_fifo_duration.tv_sec, sched_fifo_duration.tv_nsec);
                FILE *ptr;
                ptr = fopen("time.txt", "w");
                if (ptr == NULL) {
                    perror("fopen failed");
                    exit(1);
                }
                fprintf(ptr, "%ld.%09ld\n", sched_other_duration.tv_sec, sched_other_duration.tv_nsec);
                fprintf(ptr, "%ld.%09ld\n", sched_rr_duration.tv_sec, sched_rr_duration.tv_nsec);
                fprintf(ptr, "%ld.%09ld\n", sched_fifo_duration.tv_sec, sched_fifo_duration.tv_nsec);
                fclose(ptr);
                char *args[] = {"python3", "scheduler.py", NULL};
                execvp("python3", args);
                perror("execvp failed");
                exit(1);
            }
        }
    }
    return 0;
}
