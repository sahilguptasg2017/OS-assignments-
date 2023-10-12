#include <bits/types/timer_t.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <signal.h>
#include <time.h>
#include <bits/types/struct_timeval.h>
#include <sys/time.h>
#include <semaphore.h>

#define MAX_ARGC 6
#define SH_TOKEN_BUFFSIZE 70
#define MAX_SIZE 2000

#define HISTORY_SIZE 2000
#define HISTORY_ERROR_CODE -d1

#define MAX_QUEUE_SIZE 100
#define MAX_PRIORITY 4

typedef struct {
    pid_t pid;
    char command[10];
    int priority;
    char* STATE;
} Process;

typedef struct {
    int fd;
    sem_t* semaphore;
    char* memory_name;
    char* semaphore_name;
} shm_t;

typedef struct {
    Process process_table[MAX_PRIORITY][MAX_QUEUE_SIZE];
    int process_index[MAX_PRIORITY];
    Process* processes_running;
    int start_running;
    shm_t* shm;
    int value_check;
} SharedMemory;

typedef struct {
    struct sigevent sev;
    struct itimerspec* its;
    timer_t timerid;
} Timer;

// shell functions
void shell_loop(void);
char *read_line();
char **splitting_function(char *command);

// extra commands
int cd_command(char ** args);
int pwd_command(char *pwd);
void update_prompt(char* pwd);

// handlers
void handler_exit(int signum);
void handler_alarm(union sigval);

// process functions
void process_info(Process process);
void show_all_processes();

// scheduler functions
void run_scheduler();
int run_scheduled_processes(int n);
void cleanup();

// queue functions
int enqueueProcess(Process p);
Process dequeueProcess();
int queueIsEmpty();

// timer functions
Timer* create_timer();
int start_timer();
int delete_timer();

// shared memory
SharedMemory* setup_shared_memory(int NCPU);
void cleanup_shared_memory(SharedMemory* shared_memory);