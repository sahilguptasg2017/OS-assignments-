
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PASSENGERS 1000
sem_t car_capacity; 
sem_t car_load;
sem_t mutex;        
sem_t car_ride;    
sem_t mutext2;
sem_t thrcr;
sem_t max_cap;
int load_done = 0;
sem_t mutext3;
sem_t mutext8;
int board_var = 0;
pthread_cond_t loadhandle;
pthread_mutex_t loadhandler;
int ridecomplete = 0;
int total_passengers; // Total number of passengers
int car_capacity_val; // Capacity of the car
int passengers_count = 0; // Number of passengers currently on the car
struct Node {
    int data;
    struct Node* next;
};
struct Node* head;
// Function to create a new node
struct Node* createNode(int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to append a node at the end of the linked list
void append(struct Node** headRef, int data) {
    struct Node* newNode = createNode(data);
    if (*headRef == NULL) {
        *headRef = newNode;
    } else {
        struct Node* temp = *headRef;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}
void displayList(struct Node* head) {
    struct Node* current = head;
    if (current == NULL) {
        printf("The list is empty.\n");
        return;
    }
    printf("Linked List: ");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}
int find(int ID){
    struct Node* temp = head;
    while(temp!= NULL){
        if (temp->data==ID){
            return 1;
        }
        temp = temp->next;
    }
    return 0;

}
void run(){
    printf("Car is running\n");
}
int popFromStart(struct Node** headRef, int id) {
    if (*headRef == NULL) {
        printf("List is empty, nothing to delete!\n");
        return 0;
    }

    struct Node* temp = *headRef;
    struct Node* prev = NULL;

    // If the node to be deleted is the head node
    if (temp != NULL && temp->data == id) {
        *headRef = temp->next;
        free(temp);
        return 1; // Node with given ID deleted
    }

    // Find the node with the given ID
    while (temp != NULL && temp->data != id) {
        prev = temp;
        temp = temp->next;
    }

    // If the node with the given ID is found, delete it
    if (temp != NULL) {
        prev->next = temp->next;
        free(temp);
        return 1; // Node with given ID deleted
    } else {
        printf("Node with ID %d not found!\n", id);
        return 0; // Node with given ID not found
    }
}
void load() {
    // sem_wait(&mutext8);
    while (load_done==1) {
        sleep(1);
    }
    printf("Car is loaded. Starting the ride!\n");
    load_done = 1;
    for (int o =0;o<car_capacity_val;o++){
        sem_post(&car_load);
    }
    // sem_post(&mutext8);
}

void unload() {
    sem_wait(&mutext8);
    printf("Car is unloaded. Ending the ride!\n");
    load_done = 0;
    sem_post(&mutext8);
}

void board(int ID) {
    sem_wait(&car_load);
    sem_wait(&mutext8);
    while (ridecomplete && !load_done && head->next!=NULL){
 //       printf("ho\n");
        sleep(1);
    }
    if (find(ID)==1){
        return;
    }
    while (ridecomplete && !load_done && head->next!=NULL){
        // printf("ho\n");
        sleep(1);
    }
    append(&head, ID);
    passengers_count++;
    board_var = 1;
    while (ridecomplete && !load_done && head->next!=NULL){
        // printf("ho\n");
        sleep(1);
    }
    printf("Passenger %d is boarding\n", ID);
    sem_post(&mutext8);
}

int offboard(int ID) {
    sem_wait(&mutext3);
    while (ridecomplete!=1){
        sleep(1);
    }
    while (head->next==NULL){
        // printf("pi");
        // sleep(1000);
        // load();
        passengers_count = 0;
        head->next = NULL;
        ridecomplete = 0;
        // load();
        return 0;
    }
    printf("Passenger %d is getting off\n", ID);
    passengers_count--;
    // displayList(head);
    popFromStart(&head,ID);
    // pthread_exit(NULL);
    // printf("out pass%d\n",passengers_count);
    // return 1;
    sem_post(&max_cap);
    sem_post(&mutext3);
    return 1;

}

void* car(void* args) {
    while (1) {
        load();
        sem_post(&car_load);
        sem_wait(&car_ride); 
        while (passengers_count!=car_capacity_val) {
            // offboard(head->next->data);
            sleep(1);
        }
      //  printf("%d\n",passengers_count);
        run();
        sleep(3); 
        unload();
        ridecomplete = 1;
        sem_wait(&mutext2);
        for (int i = 0; i < car_capacity_val; ++i) {
            sem_post(&car_capacity);
        }
        sem_post(&mutext2);
        // passengers_count=0;
        sleep(3);
        sem_post(&car_ride);
    }
    return NULL;
}

void* passenger(void* args) {
    int passenger_id = *((int*)args);
    while (1) {
        sem_wait(&mutex); 
        // printf("%d\n",passengers_count);
        // if (passengers_count>car_capacity_val){
        //     printf("JIJI\n");
        // }
        sem_wait(&car_load);
            sem_wait(&max_cap);
            // sem_wait(&car_load);
            board(passenger_id);
            sem_post(&car_load);
            if (passengers_count == car_capacity_val) {
                sem_post(&car_ride);
            }
            sem_post(&mutex); 
            sem_wait(&car_capacity);
            // sem_wait(&mutext3);
            offboard(passenger_id);

                // pthread_e
            // sem_post(&mutext3);
            sem_post(&mutex); 
            // break;
            sem_post(&car_load); 
        }
            sem_post(&mutex);
            // sleep(3);
            // return NULL;
    return NULL;
}

int main() {
    pthread_t car_thread;
    pthread_t passenger_threads[MAX_PASSENGERS];
    head = createNode(-1);

    printf("Enter total number of passengers: ");
    scanf("%d", &total_passengers);

    printf("Enter car capacity: ");
    scanf("%d", &car_capacity_val);
    sem_init(&thrcr, 0, 1);
    sem_init(&car_capacity, 0, 0);
    sem_init(&max_cap, 0, car_capacity_val);
    sem_init(&mutex, 0, 1);
    sem_init(&mutext2, 0, 1);
    sem_init(&mutext3, 0, 1);
    sem_init(&car_ride, 0, 0);
    sem_init(&car_load, 0, 0);
    sem_init(&mutext8, 0, 1);

    if (total_passengers <= 0 || car_capacity_val <= 0 || car_capacity_val > total_passengers) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    int car_arg = 0; // Argument for car thread (dummy value 0)
    pthread_create(&car_thread, NULL, car, &car_arg); // Pass the address of car_arg as an argument
    int ct = total_passengers%car_capacity_val+1;
    int bt =total_passengers;
    int fg =0;
    int passenger_ids[MAX_PASSENGERS];
    // while (1) {
        // sem_wait(&thrcr);
        for (int i = 0; i < total_passengers; ++i) {
            passenger_ids[i] = i + 1;
            pthread_create(&passenger_threads[i], NULL, passenger, &passenger_ids[i]);
            // if (fg){
            // sleep(1);} // Add slight delay between passenger threads
        }
        // pthread_join(car_thread, NULL);
        for (int i = 0; i < total_passengers; ++i) {
            pthread_join(passenger_threads[i], NULL);
            sem_wait(&thrcr);
            // printf("%d\n",bt);
            bt--;
            if (bt<=ct){
                sem_post(&thrcr);
                break;
            }
            sem_post(&thrcr);
            fg = 1;
        }
        // sem_post(&thrcr);
    // }

    

    sem_destroy(&car_capacity);
    sem_destroy(&mutex);
    sem_destroy(&car_ride);

    return 0;
}