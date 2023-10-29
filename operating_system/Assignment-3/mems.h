/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/mman.h>


/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096
struct main_chain_node{
    struct main_chain_node* prev_main ; 
    int pages ; 
    void* mems_phy_address ;
    struct sub_chain_node* pointer_to_subchain ;
    struct main_chain_node* next_main ; 
};
struct sub_chain_node{
    struct sub_chain_node* prev_subchain ;
    int type ;  // hole = 0 , process = 1 ; 
    void* physical_address ;
    size_t start ; 
    size_t end ; 
    size_t node_size ;
    struct sub_chain_node* next_subchain ;
};
struct main_chain_node *head = NULL ;

void* mems_virtual_start_address = NULL;


/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init(){
    head = NULL;    
    mems_virtual_start_address = (void*) 0 ;
    if(mems_virtual_start_address == MAP_FAILED){
        perror("mmap failed") ;
        exit(EXIT_FAILURE) ;
    }
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
void* mems_malloc(size_t size){ // int *arr = mems_malloc(sizeof(int) * 10)
    int n ; 
    while(n*PAGE_SIZE < size){
        n++ ; 
    }
    int flag = 0;
    struct main_chain_node *current = head ;
    int total_size = 0 ;
    while (current != NULL) {
        struct sub_chain_node *current_subchain = current->pointer_to_subchain ;
        while (current_subchain != NULL) {
            // need to change virtual address here . 
            current_subchain->start = total_size ;
            current_subchain->end = total_size+current_subchain->node_size - 1 ;
            total_size+=current_subchain->node_size ;
            current_subchain=current_subchain->next_subchain;
        }       
        current=current->next_main;
    }
        
    while(current != NULL){
        struct sub_chain_node *current_subchain = current->pointer_to_subchain ;
        current_subchain->start = PAGE_SIZE*(current->pages) ;
        while(current_subchain!=NULL){
            if(current_subchain->type == 0 && current_subchain->node_size > size ){
                flag =1 ;
                size_t available = current_subchain->node_size ;
                if(current_subchain->next_subchain != NULL){
                    struct sub_chain_node *next_to_node = current_subchain->next_subchain;
                    struct sub_chain_node *new_sub_chain_node = (struct sub_chain_node*)((char*)current_subchain + size);
                    current_subchain->type = 1 ;
                    current_subchain->next_subchain = new_sub_chain_node ;
                    current_subchain->node_size = size ;
                    new_sub_chain_node->end = current_subchain->end ;
                    new_sub_chain_node->start = size ;
                    current_subchain->end = size - 1 ;
                    new_sub_chain_node->prev_subchain = current_subchain ;
                    new_sub_chain_node->next_subchain = next_to_node ;
                    new_sub_chain_node->type = 0 ;
                    new_sub_chain_node->node_size = available - size ;
                    return (void*)current_subchain->start;
                }
                else{
                    struct sub_chain_node *new_sub_chain_node = (struct sub_chain_node*)((char*)current_subchain + size);
                    current_subchain->type = 1 ;
                    current_subchain->next_subchain = new_sub_chain_node ;
                    current_subchain->node_size = size ;
                    new_sub_chain_node->end = current_subchain->end ;
                    new_sub_chain_node->start = size ;
                    current_subchain->end = size - 1 ;
                    new_sub_chain_node->prev_subchain = current_subchain ;
                    new_sub_chain_node->next_subchain = NULL ;
                    new_sub_chain_node->type = 0 ;
                    new_sub_chain_node->node_size = available - size ;
                    return (void*)current_subchain->start;
                }
            }
            else if(current_subchain->type == 0 && current_subchain->node_size == size ){
                flag = 1 ; 
                current_subchain->type = 1 ;
            }
            current_subchain = current_subchain->next_subchain ;
        }
        current = current->next_main ;
    }
    if(flag == 0){
        void* addr = mmap(NULL,n*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) ;
        struct main_chain_node *new_main = (struct main_chain_node*) mmap(NULL,n*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)  ;
        new_main->mems_phy_address = addr ;
        



    }


    

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