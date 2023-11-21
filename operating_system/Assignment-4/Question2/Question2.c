#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PASSENGERS 100

int passenger_count = 0;  // Number of passengers on the car
int capacity;             // Maximum capacity of the car
sem_t loading, unloading; // Semaphores for loading and unloading
pthread_mutex_t mutex;    // Mutex to protect passenger_count

void load() {
    printf("Car is loading passengers.\n");
    sleep(2); // Simulate loading time
}

void unload() {
    printf("Car is unloading passengers.\n");
    sleep(2); // Simulate unloading time
}

void board(int passenger_id) {
    printf("Passenger %d is boarding.\n", passenger_id);
    sleep(1); // Simulate boarding time
}

void offboard(int passenger_id) {
    printf("Passenger %d is getting off.\n", passenger_id);
    sleep(1); // Simulate getting off time
}

void *car(void *args) {
    while (1) {
        sem_wait(&loading); // Wait for passengers to load
        load();             // Load passengers
        sem_post(&unloading); // Signal passengers to unload
        unload();            // Unload passengers
    }
    return NULL;
}

void *passenger(void *args) {
    int passenger_id = *((int *)args);
    while (1) {
        pthread_mutex_lock(&mutex);
        if (passenger_count < capacity) {
            passenger_count++;
            board(passenger_id);
            pthread_mutex_unlock(&mutex);

            sem_post(&loading); // Signal car that passenger has boarded
            sem_wait(&unloading); // Wait for car to finish unloading

            offboard(passenger_id);

            pthread_mutex_lock(&mutex);
            passenger_count--;
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex);
        }

        // Sleep to simulate some time before the next attempt
        sleep(1);
    }
    return NULL;
}

int main() {
    int num_passengers;

    printf("Enter the capacity of the car: ");
    scanf("%d", &capacity);

    printf("Enter the total number of passengers: ");
    scanf("%d", &num_passengers);

    if (num_passengers > MAX_PASSENGERS) {
        printf("Number of passengers exceeds maximum limit.\n");
        return 1;
    }

    pthread_t car_thread, passenger_threads[MAX_PASSENGERS];
    int passenger_ids[MAX_PASSENGERS];

    sem_init(&loading, 0, 0);
    sem_init(&unloading, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&car_thread, NULL, car, NULL);

    for (int i = 0; i < num_passengers; i++) {
        passenger_ids[i] = i + 1;
        pthread_create(&passenger_threads[i], NULL, passenger, &passenger_ids[i]);
    }

    pthread_join(car_thread, NULL);

    for (int i = 0; i < num_passengers; i++) {
        pthread_join(passenger_threads[i], NULL);
    }

    sem_destroy(&loading);
    sem_destroy(&unloading);
    pthread_mutex_destroy(&mutex);

    return 0;
}

