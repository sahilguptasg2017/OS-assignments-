#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdlib.h>

long long get_difference(struct timespec *ts1, struct timespec *ts2)
{
    return (ts2->tv_sec - ts1->tv_sec) * 1000000000 + (ts2->tv_nsec - ts1->tv_nsec);
}

int main()
{
    // struct timespec final_time;
    int all_pid[3];
    long long all_times[3];
    struct timespec start_times[3];
    struct timespec final_times[3];

    // for(int i = 0; i < 3; i++){
    //     clock_gettime(CLOCK_REALTIME, &start_times[i]);
    //     all_pid[i] = fork();
    //     if(all_pid[i] < 0){
    //         perror("Error");
    //     }
    //     else if(all_pid[i] == 0){
    //         if(i == 0){
    //             struct sched_param param;
    //             param.sched_priority = 0;
    //             sched_setscheduler(getpid(), SCHED_OTHER, &param);
    //             char *args[] = {"./count", NULL};
    //             printf("process 1 started\n");

    //             execvp(args[0], args);
    //             perror("execvp");
    //             exit(1);
    //         }
    //         if(i == 1){
    //             struct sched_param param;
    //             param.sched_priority = 50;
    //             sched_setscheduler(getpid(), SCHED_RR, &param);
    //             char *args[] = {"./count", NULL};
    //             printf("process 2 started\n");

    //             execvp(args[0], args);
    //             perror("execvp");
    //             exit(1);
    //         }
    //         if(i == 2){
    //             struct sched_param param;
    //             param.sched_priority = 50;
    //             sched_setscheduler(getpid(), SCHED_FIFO, &param);
    //             char *args[] = {"./count", NULL};
    //             printf("process 3 started\n");

    //             execvp(args[0], args);
    //             perror("execvp");
    //             exit(1);
    //         }
    //     }
    // }

    // Process 1
    clock_gettime(CLOCK_REALTIME, &start_times[0]);
    printf("process 1 started\n");
    int rc1 = fork();
    all_pid[0] = rc1;
    if (rc1 == 0)
    {
        // Child process 1
        struct sched_param param;
        param.sched_priority = 0;
        sched_setscheduler(getpid(), SCHED_OTHER, &param);
        nice(0);
        char *args[] = {"./count", NULL};
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }

    // Process 2
    clock_gettime(CLOCK_REALTIME, &start_times[1]);
    printf("process 2 started\n");
    int rc2 = fork();

    all_pid[1] = rc2;
    if (rc2 == 0)
    {
        // Child process 2
        struct sched_param param;
        param.sched_priority = 50;
        sched_setscheduler(getpid(), SCHED_RR, &param);

        char *args[] = {"./count", NULL};
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }

    // Process 3
    clock_gettime(CLOCK_REALTIME, &start_times[2]);
    printf("process 3 started\n");
    int rc3 = fork();

    all_pid[2] = rc3;
    if (rc3 == 0)
    {
        // Child process 3
        struct sched_param param;
        param.sched_priority = 50;
        sched_setscheduler(getpid(), SCHED_FIFO, &param);

        char *args[] = {"./count", NULL};
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }

    // Parent process

    for (int i = 0; i < 3; i++)
    {
        int child_pid = waitpid(-1, NULL, 0);
        // clock_gettime(CLOCK_REALTIME, &final_time);

        for (int j = 0; j < 3; j++)
        {
            if (child_pid == all_pid[j])
            {
                clock_gettime(CLOCK_REALTIME, &final_times[j]);
                break;
            }
        }
    }

    FILE* ptr = fopen("time.txt", "w");


    for (int j = 0; j < 3; j++)
    {

        long long time_process = get_difference(&start_times[j], &final_times[j]);
        all_times[j] = time_process;
        fprintf(ptr, "%d %lld\n", j , time_process/1000);

        printf("Terminating process %d (child %d) and time(ns): %lld\n", j, all_pid[j], time_process);
    }
    


    fclose(ptr);

    return 0;
}
