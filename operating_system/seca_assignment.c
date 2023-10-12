#include "simpleScheduler.h"
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int NCPU;      // no of CPU cores
int TSLICE;    // time slice in milliseconds

SharedMemory* shared_memory;
Timer* timer;
pid_t scheduler_pid;

Process dummy_process = {-1, NULL, 0, NULL};

char PROMPT[1024] = "\033[1;33mshell_sh> \033[0m";
char HISTORY_FILE[1024];

// main
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NCPU TSLICE\n", argv[0]);
        return EXIT_FAILURE;
    }

    NCPU = atoi(argv[1]);
    TSLICE = atof(argv[2]); 
    
    printf("total number of CPU resources(NCPU): %d\ntime quantum (TSLICE in seconds): %d milliseconds\n", NCPU, TSLICE);

    // signal handling
    signal(SIGINT, handler_exit);

    shared_memory = setup_shared_memory(NCPU);

    printf("Scheduler is started as a daemon process.\n");
    scheduler_pid = fork();
    if (scheduler_pid < 0) {
        perror("Unable to initiate scheduler");
        exit(EXIT_FAILURE);
    } else if (scheduler_pid == 0) {
        // scheduler process created, will run until killed (daemon process)
        int sr, processes_left=NCPU;
        while (1) {
            printf("line number 44\n");
            sem_wait(shared_memory->shm->semaphore);
            printf("line number 46\n");
            sr = shared_memory->start_running;
            printf("%d\n",sr);
            sem_post(shared_memory->shm->semaphore);
            show_all_processes();
            printf("The value of value check is: %d\n",shared_memory->value_check);
            printf("line number 47\n");
            if (sr) {
                processes_left = run_scheduled_processes(processes_left);
            } else {
                sleep(10);
            }
        }
    } else {
        printf("In line number 55\n");
        shell_loop();
    }
    return EXIT_SUCCESS;
}

// shell functions
void shell_loop() {
    int status = 1;

    char *command;
    char **args;
    char pwd[1024];

    pwd_command(pwd);
    strcpy(HISTORY_FILE, pwd);
    strcat(HISTORY_FILE, "/history.txt");
    
    do {
        pwd_command(pwd);
        update_prompt(pwd);

        printf("%s", PROMPT);
        fflush(stdout);
        
        command = read_line();
        args = splitting_function(command);
                
        int args_count = 0;
        while (args[args_count] != NULL){
            args_count++;
        }

        int child_pid;
        int child_status = fork(), command_status;
        
        if (child_status < 0) { 
            printf("the fork command failed !");
        } else if (child_status == 0){
            signal(SIGINT, SIG_DFL);
            if (strcmp(args[0], "cd") == 0) {
                if (cd_command(args) != 1) {
                    perror("Cannot run cd command with given arguements\n");
                } else {
                    pwd_command(pwd);
                    update_prompt(pwd);
                }
            } else if (strcmp(args[0],"submit")==0) {
                if (args_count < 2 || args_count > 3) {
                    printf("Usage: \"submit\" \"command\" \"priority(default 1)\"\n");
                    continue;
                } else if (args_count == 3 && (atoi(args[2]) < 1 || atoi(args[2]) > MAX_PRIORITY)) {
                    printf("Error. priority should be in between 1 and %d\n", MAX_PRIORITY);
                    continue;
                }

                
                Process p;
                
                p.pid = 0;
                p.command = args[1];
                
                p.priority = (args_count == 2) ? 1 : atoi(args[2]);
                p.STATE = "NEW";
                int enqueued = enqueueProcess(p);
                if (enqueued == EXIT_FAILURE) {
                    printf("Cannot add more processes!\n");
                    continue;
                } else {
                    printf("Process added succesfully\n");
                    show_all_processes();
                }
            } else if (strcmp(args[0],"start") == 0) {
                // Execution is to start now
                start_timer();
                sem_wait(shared_memory->shm->semaphore);
                shared_memory->start_running = 1;
                sem_post(shared_memory->shm->semaphore);
            } else if (strcmp(args[0],"show") == 0) {
                // Show all processes in queue
                show_all_processes();
            } else {
                if (execvp(args[0], args) == -1) {
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                }
            }
        } else{
            waitpid(child_status, NULL , 0);
            free(args);
        }
    } while(status);
}


char *read_line() {
    char *command = NULL;
    size_t buffsize = 0;
    
    ssize_t read = getline(&command , &buffsize, stdin);

    if (read == -1){
        if (feof(stdin)){
            fprintf(stderr, "EOF\n");
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "Value of the errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    }
    // write_history(command);
    return command;
}

char **splitting_function(char *command) {
    int buffsize = SH_TOKEN_BUFFSIZE;
    int i= 0;
    char *tokens =(char* ) malloc(buffsize*sizeof(char *));

    char *token ;
    char *tokens_duplicate;

    if (!tokens){
        fprintf(stderr, "sh_shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(command , " \t\r\n\a");
    while (token != NULL){
        tokens[i++] = *token;

        if (i >= buffsize){
            buffsize *= 2;
            tokens_duplicate = tokens;
            tokens =*(char** )realloc(tokens, buffsize *sizeof(char *));
            
            if (!tokens){
                free(tokens_duplicate);
                fprintf(stderr, "sh_shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }

    tokens[i] = NULL;
    return &tokens;
}

// extra commands
int cd_command(char ** args) {
    if (args[1] == NULL) {
        return -1;
    } else {
        if (chdir(args[1]) != 0) {
            return 0;
        }
        return 1;
    }
}

int pwd_command(char pwd[]) {
    if (getcwd(pwd, 1024) != NULL) {
        return 1;
    } else {
        return 0;
    }
}

void update_prompt(char* pwd) {
    snprintf(PROMPT, sizeof(PROMPT), "\033[1;33mshell_sh: \033[34m%s\033[0m> ", pwd);
}

// handlers
void handler_exit(int signum) {
    // if (!queueIsEmpty()) {
    //     printf("Cannot close shell right now\n");
    //     printf("There are some processes still submitted\n");
    //     return;
    // }
    printf("\nCtrl + C was pressed! Exiting...\n");

    cleanup();
    exit(signum);
}

void handler_alarm(union sigval sig) {
    printf("2\n");
    // stop the already running processes which are stored in processes_running array.
    // put these processes back in the queue
    // call run_n_processes() to run next NPCU processes
    Process p;
    for (int i=0; i<NCPU; i++) {
        p = shared_memory->processes_running[i];

        if (strcmp(p.STATE, "TERMINATED") == 0) {
            // process already terminated
            printf("In the handler, Process %s with pid %d is %s\n", p.command, p.pid, p.STATE);
            continue;
        } else if (strcmp(p.STATE,"NEW") == 0) {
            // a NEW process entered the running queue somehow
            printf("In the handler, Process %s with pid %d is %s\n", p.command, p.pid, p.STATE);
            continue; 
        } else if (strcmp(p.STATE, "RUNNING") == 0) {
            printf("In the handler, Process %s with pid %d is %s\n", p.command, p.pid, p.STATE);
            if (kill(p.pid, SIGSTOP) == 0) { 
                shared_memory->processes_running[i].STATE = "READY";
                enqueueProcess(shared_memory->processes_running[i]);
            } else {
                printf("Some error while stopping the running process %s with pid %d\n", p.command, p.pid);
                exit(EXIT_FAILURE);
            }
        } else {
            // process is neither TERMINATED, NEW or RUNNING (either READY or something else)
            printf("In the handler, Process %s with pid %d is %s\n", p.command, p.pid, p.STATE);
        }
    }
    run_scheduled_processes(NCPU);
}

// process functions
void process_info(Process process){
    printf("-----------------------------\n"); 
    printf("Process PID is:   %d\n",process.pid);
    printf("Command is:       %s\n",process.command);
    printf("Process Priority: %d\n",process.priority);
    printf("Process State:    %s\n",process.STATE);
    printf("-----------------------------\n"); 
}

void show_all_processes() {
    printf("fvbekr\n");

    if (!queueIsEmpty()) {
        for (int i=MAX_PRIORITY-1; i>=0; i--) {
            printf("%dth process index %d\n",i,shared_memory->process_index[i]);
            for (int j=0; j<shared_memory->process_index[i]; j++) {
                printf("fvsffvjkjdscwhfk\n");
                process_info(shared_memory->process_table[i][j]);
                printf("xxxxxxxxxxxxxxxxxxxxxxxx\n");
            }
        }
    } else {
        printf("Queue empty. No process to show!\n");
    }
}

// scheduler functions
void run_scheduler() {
    printf("Scheduler is started as a daemon process.\n");
    scheduler_pid = fork();
    if (scheduler_pid < 0) {
        perror("Unable to initiate scheduler");
        exit(EXIT_FAILURE);
    } else if (scheduler_pid == 0) {
        // scheduler process created, will run until killed (daemon process)
        int sr, processes_left=NCPU;
        while (1) {
            sem_wait(shared_memory->shm->semaphore);
            sr = shared_memory->start_running;
            sem_post(shared_memory->shm->semaphore);

            if (sr) {
                processes_left = run_scheduled_processes(processes_left);
            } else {
                sleep(1);
            }
        }
    } else {
        shell_loop();
    }
}

int run_scheduled_processes(int n) {
    Process p;
    char* args[2];
    for (int i=0; i<n; i++) {
        p = dequeueProcess();
        if (p.pid == dummy_process.pid) return i;
        else {
            sem_wait(shared_memory->shm->semaphore);
            shared_memory->processes_running[i] = p;
            sem_post(shared_memory->shm->semaphore);
        }
    }

    for (int i=0; i<n; i++) {
        sem_wait(shared_memory->shm->semaphore);
        p = shared_memory->processes_running[i];
        if (strcmp(p.STATE, "NEW") == 0) {
            int child_proc = fork();
            if (child_proc < 0){
                printf("fork process failed!\n");
            } else if (child_proc == 0) {
                args[0] = p.command;
                args[1] = NULL;
                
                execvp(args[0], args);
                exit(EXIT_SUCCESS);
            } else {
                shared_memory->processes_running[i].pid = child_proc;
                shared_memory->processes_running[i].STATE = "RUNNING";
            }
        } else if (strcmp(p.STATE, "READY") == 0) {
            if (kill(p.pid, SIGCONT) == 0) {
                // continued successfully
            } else {
                // error in continuing
                printf("Error in resuming process %s with pid %d\n", p.command, p.pid);
                exit(EXIT_FAILURE);
            }
        } else {
            // Process is neither NEW nor READY
            printf("In the run scheduled process, Process %s with pid %d is %s\n", p.command, p.pid, p.STATE);
        }
        sem_post(shared_memory->shm->semaphore);
    }

    for (int i=0; i<n; i++) {
        sem_wait(shared_memory->shm->semaphore);
        waitpid(shared_memory->processes_running[i].pid, NULL, 0);
        shared_memory->processes_running[i].STATE = "TERMINATED";
        sem_post(shared_memory->shm->semaphore);
    }

    return NCPU;
}

void cleanup() {
    // kill the scheduler process if the queue is empty
    kill(scheduler_pid, SIGTERM);

    delete_timer();
    free(timer);
    cleanup_shared_memory(shared_memory);
}

// Queue functions
int enqueueProcess(Process p) {
    int index = p.priority - 1;
    // process_info(p);
    sem_wait(shared_memory->shm->semaphore);
    // printf("dbrgfbdb\n");
    if (shared_memory->process_index[index] == MAX_QUEUE_SIZE) return EXIT_FAILURE;
    shared_memory->process_table[index][shared_memory->process_index[index]++] = p;
    // process_info(shared_memory->process_table[index][shared_memory->process_index[index]]);
    // show_all_processes();
    shared_memory->value_check +=1;
    sem_post(shared_memory->shm->semaphore);
    return EXIT_SUCCESS;
}

Process dequeueProcess() {
    int hPriority = -1; // highest priority
    for (int i=MAX_PRIORITY-1; i>=0; i--) {
        if (shared_memory->process_index[i] > 0) {
            hPriority = i;
            break;
        }
    }
    
    if (hPriority == -1) {
        return dummy_process;
    }
    
    sem_wait(shared_memory->shm->semaphore);
    Process p = shared_memory->process_table[hPriority][0];

    // Shifting the remaining to right
    for (int i = 0; i < shared_memory->process_index[hPriority] - 1; i++) {
        shared_memory->process_table[hPriority][i] = shared_memory->process_table[hPriority][i+1];
    }
    
    shared_memory->process_index[hPriority]--;
    sem_post(shared_memory->shm->semaphore);
    return p;
}

int queueIsEmpty() {
    int empty=1;
    for (int i=MAX_PRIORITY-1; i>=0; i--) {
        if (shared_memory->process_index[i] > 0) {
            empty = 0;
            break;
        }
    }

    return empty;
}

//timer functions
Timer* create_timer() {
    Timer* t = (Timer*) malloc(sizeof(Timer));
    if (t == NULL) exit(EXIT_FAILURE);

    struct sigevent sev;
    struct itimerspec its;
    timer_t timerid;

    // Create a timer
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = handler_alarm;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &timerid;

    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    int millisec = TSLICE;
    int second = millisec / 1000;
    its.it_value.tv_sec = second;
    its.it_value.tv_nsec = (millisec - (second * 1000)) * 1000;
    its.it_interval.tv_sec = second;
    its.it_interval.tv_nsec = (millisec - (second * 1000)) * 1000;

    // Start the timer
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    t->timerid = timerid;
    t->its = &its;
    t->sev = sev;

    return t;
}

int start_timer() {
    // returns -1 if fails
    return timer_settime(timer->timerid, 0, timer->its, NULL);
}

int delete_timer() {
    // returns -1 if fails
    return timer_delete(timer->timerid);
}

// shared memory
SharedMemory* setup_shared_memory(int NCPU) {
    int shm_fd;
    char sem_name[] = "/my_semaphore";
    char mem_name[] = "/my_shared_memory";
    sem_t* sem;
    SharedMemory* shared_memory;
    

    // create a shared memory object
    shm_fd = shm_open(mem_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // set the size of shared memory
    if (ftruncate(shm_fd, sizeof(SharedMemory)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // map the shared memory
    shared_memory = (SharedMemory* ) mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // create semaphore
    sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    shared_memory->shm = (shm_t*) malloc(sizeof(shm_t));
    shared_memory->shm->semaphore = sem;
    shared_memory->shm->memory_name = mem_name;
    shared_memory->shm->semaphore_name = sem_name;

    // initialise the process table and all
    
    for (int i=0; i<MAX_PRIORITY; i++) shared_memory->process_index[i] = 0;
    shared_memory->processes_running = (Process*) malloc(NCPU * sizeof(Process));
    shared_memory->start_running = 0;
    shared_memory->value_check = 0;

    return shared_memory;
}   

void cleanup_shared_memory(SharedMemory* shared_memory) {
    for (int i=0; i<MAX_PRIORITY; i++) free(shared_memory->process_table[i]);
    free(shared_memory->process_table);
    free(shared_memory->processes_running);

    if (munmap(shared_memory, sizeof(SharedMemory)) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if (close(shared_memory->shm->fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(shared_memory->shm->memory_name) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    if (sem_close(shared_memory->shm->semaphore) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(shared_memory->shm->semaphore_name) == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    free(shared_memory->shm);
}
