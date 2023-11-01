/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/mman.h>

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096
typedef struct main_node{
    
    struct main_node* prev ;
    int pages ; 
    unsigned long virtual_start ; 
    unsigned long virtual_end ;    
    void* phy_addr ;
    struct subchain_node* subchain ;
    struct main_node* next ;

}main_node;

typedef struct subchain_node{
    struct subchain_node* prev ;
    int type ; // 0 for hole 1 for process .. 
    unsigned long virtual_start ; 
    unsigned long virtual_end ;
    void* phys_addr ;    
    size_t size ; 
    struct subchain_node* next ;
      
}subchain_node; 

unsigned long* vir_address ;
struct main_node* head ;
struct main_node* current_pointer ;
unsigned long  main_node_size = sizeof(struct main_node) ;
unsigned long subchain_size = sizeof(struct subchain_node) ;
 

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init(){
    vir_address = (unsigned long*)1000 ;
    head = (main_node*)mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE , -1, 0);
    head->next = NULL ;
    current_pointer=head ;


}


/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    struct main_node* main_chain = head ;
    while(main_chain != NULL){
        munmap(&main_chain,main_chain->pages*PAGE_SIZE) ;
        main_chain = main_chain->next ;
    }
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
void* mems_malloc(size_t size){
    int n = 1 ; 
    while(n*PAGE_SIZE < size){
        n++ ;
    }

    printf("mems_mall\n");

    if(size == 0){
        return NULL;
    }
    else{
        //printf("etre\n");
        if(head->next != NULL){
            printf("head != NULL\n") ;
            main_node* main_chain = head ;
            while (main_chain!=NULL) {
                subchain_node* sub_chain = main_chain->subchain;
                //need to increase the current_pointer // 
                while (sub_chain!=NULL) {
                    if(sub_chain->type == 0 && sub_chain->size>size){
                        
                    }
                    else if (sub_chain->type == 0 && sub_chain->size==size){
                        sub_chain->type = 1 ;
                        



                    }


                    sub_chain= sub_chain->next ;
                }
                


            }
        }
        else{
            //printf("dfkoaed\n");
            main_node* first_node = current_pointer+sizeof(struct main_node) ;
            first_node->phy_addr = mmap(NULL,n*PAGE_SIZE, PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE ,-1,0) ;
            //printf("fefe");     
            head->next = first_node ;
            first_node->pages = n ;
            first_node->virtual_start = vir_address;
            first_node->virtual_end = first_node-> virtual_start+n*PAGE_SIZE - 1 ;
            subchain_node* subchain = (subchain_node*)mmap(NULL,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE ,-1,0) ;
            first_node->subchain = subchain ;
            subchain->phys_addr = first_node->phy_addr ;
            
                
        }


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