#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t forks[5];
pthread_mutex_t bowl[2];
pthread_mutex_t mutex;
pthread_cond_t bowl_condition[2];
int bowl_available[2] = {1, 1};

int left(int p) {
    return p;
}

int right(int p) {
    return (p + 1) % 5;
}

void eating(int id) {
    printf("Philosopher %d is eating right now.\n", id);
    sleep(2);
}

void thinking(int id) {
    printf("Philosopher %d is thinking right now.\n", id);
    sleep(1);
}

void* philosopher(void* args) {
    int id = *(int*)args;
    int left_fork = left(id);
    int right_fork = right(id);
    while (1) {
        thinking(id);

        pthread_mutex_lock(&forks[left_fork]);
        pthread_mutex_lock(&forks[right_fork]);

        int bowl_id = -1;
        pthread_mutex_lock(&mutex);
        while (bowl_id == -1) {
            for (int i = 0; i < 2; i++) {
                if (bowl_available[i]) {
                    bowl_id = i;
                    bowl_available[i] = 0;
                    pthread_mutex_lock(&bowl[i]);
                    break;
                }
            }
     //       printf("%d\n",bowl_id) ;
            if (bowl_id == -1) {
                pthread_mutex_unlock(&forks[right_fork]);
                pthread_mutex_unlock(&forks[left_fork]);

                pthread_cond_wait(&bowl_condition[0], &mutex);
            }
        }
        pthread_mutex_unlock(&mutex);

        eating(id);

        pthread_mutex_unlock(&bowl[bowl_id]);
        pthread_mutex_lock(&mutex);
        bowl_available[bowl_id] = 1;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&bowl_condition[0]);

        pthread_mutex_unlock(&forks[right_fork]);
        pthread_mutex_unlock(&forks[left_fork]);
    }
}

int main() {
    pthread_t philosophers[5]; // 5 threads for philosophers ..
    int id[5];
    
    for (int i = 0; i < 5; i++) {
        if (pthread_mutex_init(&forks[i], NULL) != 0) {
            perror("Error initializing fork mutex");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 2; i++) {
        if (pthread_cond_init(&bowl_condition[i], NULL) != 0) {
            perror("Error initializing bowl condition variable");
            exit(EXIT_FAILURE);
        }
        if (pthread_mutex_init(&bowl[i], NULL) != 0) {
            perror("Error initializing bowl mutex");
            exit(EXIT_FAILURE);
        }
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Error initializing main mutex");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; i++) {
        id[i] = i;
        if (pthread_create(&philosophers[i], NULL, philosopher, &id[i]) != 0) {
            perror("Error creating philosopher thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (pthread_join(philosophers[i], NULL) != 0) {
            perror("Error joining philosopher thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (pthread_mutex_destroy(&forks[i]) != 0) {
            perror("Error destroying fork mutex");
        }
    }

    for (int i = 0; i < 2; i++) {
        if (pthread_mutex_destroy(&bowl[i]) != 0) {
            perror("Error destroying bowl mutex");
        }
        if (pthread_cond_destroy(&bowl_condition[i]) != 0) {
            perror("Error destroying bowl condition variable");
        }
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
