#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lab4.h"


Page_Entry* page_table;
TLB_Table tlb;
Main_Memory main_mem;
FILE* backing_store_fp;
PAGE_SIZE = 12288/4096;

// Implement get_page_from() and get_offset_from() functions below this line
int get_page_from(int virtual_address){
    return virtual_address / PAGE_SIZE;
}
int get_offset_from(int virtual_address){
    return virtual_address % PAGE_SIZE;
}

// Implement the page table functions below this line
void page_table_init();{
    page_table = (Page_Entry *)
malloc(sizeof(Page_Entry)* NUM_PAGES);
    if(page_table == NULL){
        perror("Memory allocation failed for the page table ");
        exit(1)
    }
    for(int i = 0; i < NUM_PAGES; i++){
    page_table[i].frame_number = -1;
}
Page_Entry* page_table_get_entry(int page_number) {
    if (page_number < 0 || page_number >= NUM_PAGES) {
        fprintf(stderr, "Invalid page number: %d\n", page_number);
        return NULL;
    }
    return &page_table[page_number];
    }
}
void page_table_set_entry(int page_number, int frame_number) {
    if(page_number < 0 || page_number >= NUM_PAGES){
        fprintf(stderr,"Invaild page number: %d\n",page_number);
        return;
    }
    page_table[page_number].frame_number = frame_number;
}
    
// Implement the TLB functions below this line
void tlb_init(){
    tlb.entries = (TLB_Entry*) malloc(sizeof(TLB_Entry)*TLB_SIZE);
}


// Implement the Physical Memory functions below this line



// Implement the Backing Store functions below this line