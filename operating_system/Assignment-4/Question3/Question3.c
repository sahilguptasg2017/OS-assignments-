
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CARS 100
#define MAX_ON_BRIDGE 5
// sem_t bin;
struct Node {
    int data;
    struct Node* next;
};

// Function to create a new node
struct Node* createNode(int value) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->data = value;
    newNode->next = NULL;
    return newNode;
}


void append(struct Node** head, int value) {
    struct Node* newNode = createNode(value);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    struct Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}
void pop(struct Node** head) {
        if (*head == NULL || (*head)->next == NULL) {
        printf("Linked list doesn't have enough elements to delete the second node.\n");
        return;
    }
    struct Node* temp = (*head)->next;
    (*head)->next = temp->next;
    free(temp);
}
void displayList(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}


sem_t bridge;
sem_t mutex;
sem_t mutext2;
int cars_on_bridge = 0;
int waiting_left = 0;
struct Node * headl;
struct Node * headr;
pthread_mutex_t leftlock;
pthread_mutex_t rightlock;
pthread_mutex_t lock2;
int waiting_right = 0;
int tmpr=5;
// void pr(){
//     int k =0;
//     while (k<MAX_CARS) {
//         printf("%d ",buff[k
//         ]);
//         k++;
//     }
// }
int tmpl=5;
void passing(int dir, int car_num) {
    if (dir == 1) {
        sem_wait(&mutex);
        waiting_left++;
        sem_post(&mutex);
        sem_wait(&bridge);
        while(tmpr<5 || tmpl<=0){
            sleep(
                3);
        }
        pthread_mutex_lock(&leftlock);
        printf("Car from left side with ID %d is crossing the bridge.\n", car_num);
        sem_wait(&mutext2);
        struct Node *node = headl;
        append(&headl, car_num);
        // buff[car_num-1] =1;
        sem_post(&mutext2);
        // pr();
        tmpl--;
        // sem_post(&mutext2);
        pthread_mutex_unlock(&leftlock);
        sleep(1);
        // sem_wait(&mutext2);
        pthread_mutex_lock(&leftlock);
        if (headl->next->data==car_num){
            sem_wait(&mutext2);
            pop(&headl);
            // displayList(headl);
            sem_post(&mutext2);    
        }
        else{
            // pthread_mutex_lock(&mutext2);
            sem_wait(&mutext2);
            struct Node* n = headl;
            // append(&headl, car_num);
            car_num = headl->next->data;
            pop(&headl);
            // displayList(headl);
            sem_post(&mutext2);
        }
        pthread_mutex_unlock(&leftlock);
        // sem_post(&mutext2);
        // pr();
        sem_wait(&mutext2);
        pthread_mutex_lock(&lock2);
        printf("Car from left side with ID %d crossed the bridge.\n", car_num);
        pthread_mutex_unlock(&lock2);
        sem_post(&mutext2);
        tmpl++;
        pthread_mutex_unlock(&leftlock);
        sem_wait(&mutex);
        waiting_left--;
        sem_post(&mutex);

        sem_post(&bridge);
    } else {
        sem_wait(&mutex);
        waiting_right++;
        sem_post(&mutex);

        sem_wait(&bridge);
        while(tmpl<5 || tmpr<=0){
            sleep(1);
        }
        pthread_mutex_lock(&rightlock);
        printf("Car from right side with ID %d is crossing the bridge.\n", car_num);
        sem_wait(&mutext2);
        // buff2[car_num-1] =1;
        struct Node *node = headr;
        append(&headr, car_num);
        sem_post(&mutext2);
        tmpr--;
        pthread_mutex_unlock(&rightlock);
        sleep(1);
        // pthread_mutex_lock(&rightlock);
        pthread_mutex_lock(&rightlock);
        if (headr->next->data==car_num){
            sem_wait(&mutext2);
            pop(&headr); 
            sem_post(&mutext2)  ;
        }
        else{
            sem_wait(&mutext2);
            struct Node* n = headr;
            // append(&headr, car_num);
            car_num = headr->next->data;
            pop(&headr);
            sem_post(&mutext2);
        }
        pthread_mutex_unlock(&rightlock);
        // if (flag==0)
        sem_wait(&mutext2);
        printf("Car from right side with ID %d crossed the bridge.\n", car_num);
        sem_post(&mutext2);
        tmpr++;
        // pthread_mutex_unlock(&rightlock);

        sem_wait(&mutex);
        waiting_right--;
        sem_post(&mutex);

        sem_post(&bridge);
    }
}

void *left(void *args) {
    int car_num = *((int *)args);
    passing(1, car_num);
    pthread_exit(NULL);
}

void *right(void *args) {
    int car_num = *((int *)args);
    passing(2, car_num);
    pthread_exit(NULL);
}

int main() {
    int num_left, num_right;
    printf("Enter the number of cars on the left side: ");
    scanf("%d", &num_left);
    printf("Enter the number of cars on the right side: ");
    scanf("%d", &num_right);
    headl = createNode(0);
    headr = createNode(0);
    pthread_t left_threads[MAX_CARS], right_threads[MAX_CARS];
    sem_init(&bridge, 0, MAX_ON_BRIDGE);
    sem_init(&mutex, 0, 1);
    sem_init(&mutext2, 0, 1);
    pthread_mutex_init(&leftlock,NULL);
    pthread_mutex_init(&rightlock,NULL);
    pthread_mutex_init(&lock2,NULL);
    for (int i = 0; i < num_left; ++i) {
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i + 1;
        pthread_create(&left_threads[i], NULL, left, arg);
    }

    for (int i = 0; i < num_right; ++i) {
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i + 1;
        pthread_create(&right_threads[i], NULL, right, arg);
    }

    for (int i = 0; i < num_left; ++i) {
        pthread_join(left_threads[i], NULL);
    }

    for (int i = 0; i < num_right; ++i) {
        pthread_join(right_threads[i], NULL);
    }

    sem_destroy(&bridge);
    sem_destroy(&mutex);

    return 0;
}
