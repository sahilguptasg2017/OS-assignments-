#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

// Define constants
#define PAGE_SIZE 4096  // Page size (adjust as needed)

// Structure for a segment in the sub-chain
struct SubChainNode {
    void* address;          // Starting address of the segment
    size_t size;            // Size of the segment
    int isProcess;          // 1 for PROCESS, 0 for HOLE
    struct SubChainNode* prev;
    struct SubChainNode* next;
};

// Structure for a node in the main chain
struct MainChainNode {
    struct SubChainNode* subChainHead;
    struct MainChainNode* prev;
    struct MainChainNode* next;
};

// Global variables
struct MainChainNode* freeListHead = NULL;
void* memsVirtualAddress = NULL;

// Function to initialize MeMS
void mems_init() {
    // Initialize free list
    freeListHead = (struct MainChainNode*)malloc(sizeof(struct MainChainNode));
    freeListHead->subChainHead = NULL;
    freeListHead->prev = NULL;
    freeListHead->next = NULL;

    // Initialize other variables as needed
    memsVirtualAddress = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

// Function to finish MeMS
void mems_finish() {
    // Unmap allocated memory using munmap
    if (memsVirtualAddress != NULL) {
        munmap(memsVirtualAddress, PAGE_SIZE);
        memsVirtualAddress = NULL;
    }

    // Free the entire free list
    struct MainChainNode* currentNode = freeListHead;
    while (currentNode != NULL) {
        struct SubChainNode* subChainNode = currentNode->subChainHead;
        while (subChainNode != NULL) {
            struct SubChainNode* next = subChainNode->next;
            free(subChainNode);
            subChainNode = next;
        }
        struct MainChainNode* next = currentNode->next;
        free(currentNode);
        currentNode = next;
    }
}

// Function to allocate memory
void* mems_malloc(size_t size) {
    // Find a suitable segment in the free list or request more memory from the OS
    struct MainChainNode* currentNode = freeListHead;
    struct SubChainNode* currentSubNode;
    
    while (currentNode != NULL) {
        currentSubNode = currentNode->subChainHead;
        while (currentSubNode != NULL) {
            if (currentSubNode->isProcess == 0 && currentSubNode->size >= size) {
                // We found a large enough HOLE segment in the sub-chain
                if (currentSubNode->size > size) {
                    // Split the segment into a PROCESS and a new HOLE
                    struct SubChainNode* newHole = (struct SubChainNode*)malloc(sizeof(struct SubChainNode));
                    newHole->address = (void*)((char*)currentSubNode->address + size);
                    newHole->size = currentSubNode->size - size;
                    newHole->isProcess = 0;
                    newHole->prev = currentSubNode;
                    newHole->next = currentSubNode->next;
                    if (currentSubNode->next != NULL) {
                        currentSubNode->next->prev = newHole;
                    }
                    currentSubNode->size = size;
                    currentSubNode->isProcess = 1;
                    currentSubNode->next = newHole;
                } else {
                    // Use the entire segment as PROCESS
                    currentSubNode->isProcess = 1;
                }
                // Return the MeMS virtual address
                return currentSubNode->address;
            }
            currentSubNode = currentSubNode->next;
        }
        currentNode = currentNode->next;
    }

    // No suitable segment found, request more memory from the OS
    void* newMmapAddress = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (newMmapAddress == MAP_FAILED) {
        return NULL; // mmap failed
    }

    // Create a new node in the main chain and a single segment in its sub-chain
    struct MainChainNode* newMainNode = (struct MainChainNode*)malloc(sizeof(struct MainChainNode));
    newMainNode->subChainHead = (struct SubChainNode*)malloc(sizeof(struct SubChainNode));
    newMainNode->subChainHead->address = newMmapAddress;
    newMainNode->subChainHead->size = PAGE_SIZE;
    newMainNode->subChainHead->isProcess = 1;
    newMainNode->subChainHead->prev = NULL;
    newMainNode->subChainHead->next = NULL;
    newMainNode->prev = currentNode;
    newMainNode->next = NULL;

    if (currentNode != NULL) {
        currentNode->next = newMainNode;
    } else {
        freeListHead = newMainNode;
    }

    // Return the MeMS virtual address
    return newMmapAddress;
}

// Function to free memory
void mems_free(void* ptr) {
    // Find the segment in the free list, mark it as HOLE
    struct MainChainNode* currentNode = freeListHead;
    struct SubChainNode* currentSubNode;
    
    while (currentNode != NULL) {
        currentSubNode = currentNode->subChainHead;
        while (currentSubNode != NULL) {
            if (currentSubNode->address == ptr && currentSubNode->isProcess == 1) {
                // Mark the segment as HOLE
                currentSubNode->isProcess = 0;
                // Merge adjacent HOLE segments
                if (currentSubNode->prev != NULL && currentSubNode->prev->isProcess == 0) {
                    currentSubNode->prev->size += currentSubNode->size;
                    currentSubNode->prev->next = currentSubNode->next;
                    if (currentSubNode->next != NULL) {
                        currentSubNode->next->prev = currentSubNode->prev;
                    }
                    free(currentSubNode);
                    currentSubNode = currentSubNode->prev;
                }
                if (currentSubNode->next != NULL && currentSubNode->next->isProcess == 0) {
                    currentSubNode->size += currentSubNode->next->size;
                    currentSubNode->next = currentSubNode->next->next;
                    if (currentSubNode->next != NULL) {
                        currentSubNode->next->prev = currentSubNode;
                    }
                    free(currentSubNode->next);
                }
                return; // Memory freed successfully
            }
            currentSubNode = currentSubNode->next;
        }
        currentNode = currentNode->next;
    }
}

// Function to print MeMS statistics
void mems_print_stats() {
    // Implement code to print statistics, including mapped pages and unused memory
    int mappedPages = 0;
    size_t unusedMemory = 0;
    
    struct MainChainNode* currentNode = freeListHead;
    while (currentNode != NULL) {
        struct SubChainNode* currentSubNode = currentNode->subChainHead;
        while (currentSubNode != NULL) {
            if (currentSubNode->isProcess == 0) {
                unusedMemory += currentSubNode->size;
            }
            currentSubNode = currentSubNode->next;
        }
        mappedPages++;
        currentNode = currentNode->next;
    }

    printf("Mapped Pages: %d\n", mappedPages);
    printf("Unused Memory: %lu bytes\n", unusedMemory);
}

// Function to get MeMS physical address mapped to a virtual address
void* mems_get(void* v_ptr) {
    // Find the corresponding MeMS physical address
    struct MainChainNode* currentNode = freeListHead;
    struct SubChainNode* currentSubNode;
    
    while (currentNode != NULL) {
        currentSubNode = currentNode->subChainHead;
        while (currentSubNode != NULL) {
            if (currentSubNode->address == v_ptr) {
                return (void*)((uintptr_t)memsVirtualAddress + ((uintptr_t)currentSubNode->address - (uintptr_t)freeListHead->subChainHead->address));
            }
            currentSubNode = currentSubNode->next;
        }
        currentNode = currentNode->next;
    }

    return NULL; // Address not found
}
