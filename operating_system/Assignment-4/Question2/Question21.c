#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PASSENGERS 10

sem_t mutex, carQueue, passengerQueue;
int passengers_on_board = 0;
int total_passengers;

void* car_thread(void* arg) {
    while (1) {
        // Load passengers
        printf("Car loading...\n");
        for (int i = 0; i < total_passengers; ++i) {
            sem_post(&passengerQueue);
        }

        sem_wait(&carQueue); // Wait for all passengers to board

        // Run the ride
        printf("Car running...\n");
        sleep(2); // Simulate ride duration

        // Unload passengers
        printf("Car unloading...\n");
        for (int i = 0; i < total_passengers; ++i) {
            sem_wait(&passengerQueue);
        }

        sem_post(&carQueue); // Signal that ride is over
    }
}

void* passenger_thread(void* arg) {
    int passenger_id = *(int*)arg;

    // Board the ride
    sem_wait(&mutex);
    printf("Passenger %d boarding...\n", passenger_id);
    sem_post(&mutex);
    
    sem_post(&carQueue); // Notify the car that a passenger has boarded

    // Wait for the ride to finish
    sem_wait(&passengerQueue);

    // Get off the ride
    sem_wait(&mutex);
    printf("Passenger %d getting off...\n", passenger_id);
    sem_post(&mutex);

    sem_post(&passengerQueue); // Notify the car that a passenger has disembarked
}

int main() {
    printf("Enter the total number of passengers: ");
    scanf("%d", &total_passengers);

    if (total_passengers <= 0 || total_passengers > MAX_PASSENGERS) {
        printf("Invalid number of passengers.\n");
        return 1;
    }

    sem_init(&mutex, 0, 1);
    sem_init(&carQueue, 0, 0);
    sem_init(&passengerQueue, 0, 0);

    pthread_t car, passengers[MAX_PASSENGERS];

    pthread_create(&car, NULL, car_thread, NULL);

    int passenger_ids[MAX_PASSENGERS];
    for (int i = 0; i < total_passengers; ++i) {
        passenger_ids[i] = i + 1;
        pthread_create(&passengers[i], NULL, passenger_thread, &passenger_ids[i]);
    }

    pthread_join(car, NULL);

    for (int i = 0; i < total_passengers; ++i) {
        pthread_join(passengers[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&carQueue);
    sem_destroy(&passengerQueue);

    return 0;
}
