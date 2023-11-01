#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/mman.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

typedef struct Segment {
    size_t size;
    int type; // 0 for HOLE, 1 for PROCESS
    struct Segment* next;
    struct Segment* prev;
} Segment;

typedef struct Node {
    struct Segment* sub_chain;
    struct Node* next;
    struct Node* prev;
} Node;

Node* free_list_head = NULL;
void* mems_heap_start = NULL;

void mems_init() {
    // Initialize MeMS parameters here
    mems_heap_start = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    free_list_head = (Node*)mems_heap_start;
    free_list_head->sub_chain = NULL;
    free_list_head->next = NULL;
    free_list_head->prev = NULL;
}

void mems_finish() {
    // Unmap allocated memory using mu  nmap system call
    munmap(mems_heap_start, PAGE_SIZE);
}

void* mems_malloc(size_t size) {
    // Implement memory allocation logic here
    size_t rounded_size = ((size + sizeof(Segment) - 1) / PAGE_SIZE + 1) * PAGE_SIZE;
    
    Node* node = free_list_head;
    while (node != NULL) {
        Segment* segment = node->sub_chain;
        while (segment != NULL) {
            if (segment->type == 0 && segment->size >= rounded_size) {
                if (segment->size > rounded_size) {
                    // Split the hole into a PROCESS and a HOLE segment
                    Segment* process_segment = (Segment*)((char*)segment + rounded_size);
                    process_segment->size = segment->size - rounded_size;
                    process_segment->type = 0;
                    process_segment->next = segment->next;
                    process_segment->prev = segment;
                    if (segment->next != NULL) {
                        segment->next->prev = process_segment;
                    }
                    segment->next = process_segment;
                    segment->size = rounded_size;
                }
                
                segment->type = 1; // Mark as PROCESS
                return (void*)((char*)segment + sizeof(Segment));
            }
            segment = segment->next;
        }
        node = node->next;
    }
    
    // If no suitable hole is found, request more memory from the OS
    size_t total_size = rounded_size + sizeof(Node) + sizeof(Segment);
    Node* new_node = (Node*)mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    new_node->sub_chain = (Segment*)((char*)new_node + sizeof(Node));
    new_node->sub_chain->size = rounded_size;
    new_node->sub_chain->type = 1; // Mark as PROCESS
    new_node->sub_chain->next = NULL;
    new_node->sub_chain->prev = NULL;
    new_node->next = free_list_head;
    new_node->prev = NULL;
    if (free_list_head != NULL) {
        free_list_head->prev = new_node;
    }
    free_list_head = new_node;
    
    return (void*)((char*)new_node->sub_chain + sizeof(Segment));
}

void mems_free(void* ptr) {
    // Implement memory deallocation logic here
    Segment* segment = (Segment*)((char*)ptr - sizeof(Segment));
    segment->type = 0; // Mark as HOLE
    
    // Merge adjacent HOLEs in the free list
    Node* node = free_list_head;
    while (node != NULL) {
        Segment* current = node->sub_chain;
        while (current != NULL && current->next != NULL) {
            if (current->type == 0 && current->next->type == 0) {
                // Merge current HOLE with the next HOLE
                current->size += current->next->size + sizeof(Segment);
                current->next = current->next->next;
                if (current->next != NULL) {
                    current->next->prev = current;
                }
            }
            current = current->next;
        }
        node = node->next;
    }
}
void mems_print_stats() {
    int mapped_pages = 0;
    size_t unused_memory = 0;

    Node* node = free_list_head;
    while (node != NULL) {
        Segment* segment = node->sub_chain;
        while (segment != NULL) {
            mapped_pages += (segment->size + sizeof(Segment) - 1) / PAGE_SIZE;
            if (segment->type == 0) {
                unused_memory += segment->size;
            }
            segment = segment->next;
        }
        node = node->next;
    }

    printf("Total Mapped Pages: %d\n", mapped_pages);
    printf("Unused Memory: %zu bytes\n", unused_memory);
}

void* mems_get(void* v_ptr) {
    // Find the corresponding Segment from the virtual address
    Segment* segment = (Segment*)((char*)v_ptr - sizeof(Segment));
    
    // Calculate the corresponding physical address
    void* physical_address = (void*)((char*)segment - ((char*)segment->prev + segment->prev->size));

    return physical_address;
}