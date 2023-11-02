/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>


/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096
typedef struct subchain_node {
    struct subchain_node* prev;
    int type; // 0 for hole, 1 for process
    unsigned long virtual_start;
    unsigned long virtual_end;
    void* phys_addr;
    size_t size;
    struct subchain_node* next;
} subchain_node;

typedef struct main_node {
    struct main_node* prev;
    int pages;
    unsigned long virtual_start;
    unsigned long virtual_end;
    void* phy_addr;
    struct subchain_node* subchain;
    struct main_node* next;
} main_node;

unsigned long vir_address;
struct main_node* head;
struct main_node* current_pointer;
unsigned long main_node_size = sizeof(struct main_node);
unsigned long subchain_size = sizeof(struct subchain_node);
struct subchain_node* current_pointer_subchain;



void split_hole(subchain_node* hole, size_t size) {
    if (hole->size == size) {
        hole->type = 1;
        return;  // No need to split if the sizes are the same.
    }

    // Check if the current pointer within the subchain node exceeds the limit of PAGE_SIZE
    if ((unsigned char*)current_pointer_subchain + subchain_size >= (unsigned char*)current_pointer_subchain + PAGE_SIZE) {
        // Allocate a new subchain node using mmap
        subchain_node* new_subchain = (subchain_node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        new_subchain->prev = current_pointer_subchain;
        new_subchain->next = NULL;
        current_pointer_subchain->next = new_subchain;
        current_pointer_subchain = new_subchain;
    }

    // Calculate the virtual address and physical address for the new hole
    unsigned long new_virtual_start = hole->virtual_start + size;
    void* new_phys_addr = hole->phys_addr + size;

    // Create a new hole within the current subchain node
    subchain_node* new_hole = (subchain_node*)new_virtual_start;
    new_hole->prev = hole;
    new_hole->next = hole->next;
    new_hole->type = 0;
    new_hole->virtual_start = new_virtual_start;
    new_hole->virtual_end = hole->virtual_end;
    new_hole->size = hole->size - size;
    new_hole->phys_addr = new_phys_addr;

    hole->next = new_hole;
    hole->virtual_end = new_virtual_start - 1;
    hole->size = size;

    hole->type = 1;  // Mark the first part as a process
}




/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init() {
    vir_address = 1000;
    head = (main_node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    head->next = NULL;
    current_pointer = head;
    current_pointer_subchain = NULL;
}


/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    
}


/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 
void* mems_malloc(size_t size) {
    int n = 1;
    while (n * PAGE_SIZE < size) {
        n++;
    }

    if (size == 0) {
        return NULL;
    } else {
        if (head->next != NULL) {
            main_node* main_chain = head;
            while (main_chain != NULL) {
                subchain_node* sub_chain = main_chain->subchain;
                while (sub_chain != NULL) {
                    if (sub_chain->type == 0 && sub_chain->size >= size) {
                        if (sub_chain->size > size) {
                            // Split the hole into a process and a new hole
                            split_hole(sub_chain, size);
                        }
                        sub_chain->type = 1;
                        return (void*)sub_chain->virtual_start;
                    }
                    sub_chain = sub_chain->next;
                }
                main_chain = main_chain->next;
            }
        } else {
            // Check if the current pointer is already at the limit of PAGE_SIZE
            if ((unsigned char*)current_pointer + main_node_size >= (unsigned char*)current_pointer + PAGE_SIZE) {
                // Allocate a new main chain node using mmap
                main_node* new_node = (main_node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
                new_node->phy_addr = mmap(NULL , PAGE_SIZE*n ,PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) ;
                current_pointer->next = new_node;
                new_node->prev = current_pointer;
                current_pointer = new_node;
            }

            // Allocate a new subchain node using mmap
            subchain_node* subchain = (subchain_node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            current_pointer->subchain = subchain;
            subchain->phys_addr = current_pointer->phy_addr;
            subchain->size = PAGE_SIZE;
            subchain->type = 0;
            subchain->virtual_start = vir_address;
            subchain->virtual_end = vir_address + PAGE_SIZE - 1;
            current_pointer_subchain = subchain;

            if (subchain->size >= size) {
                if (subchain->size > size) {
                    // Split the hole into a process and a new hole
                    split_hole(subchain, size);
                }
                subchain->type = 1;
                return (void*)subchain->virtual_start;
            }
        }
    }
    return NULL;
}


/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats(){

}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
    
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    
}