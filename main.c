#include <stdio.h>
#include "lab4.h"

int main(int argc, char** argv){
    FILE* logical_address_fp = NULL;
    int logical_address;
    int physical_address;
    unsigned char page;
    unsigned char offset;
    short frame;

    if (argc < 2){
        fprintf(stderr,"SYNOPSIS: %s <addresses file>\n",argv[0]);
        return 1;
    }
    
    logical_address_fp = fopen(argv[1], "r");
    if (logical_address_fp == NULL){
        fprintf(stderr,"Error opening file: %s",argv[1]);
        return 2;
    }

    init_page_table();
    init_tlb();
    init_physical_memory();
    init_backing_store("BACKING_STORE.bin");

    int is_done = 0;
    while (!is_done){
        int num_matched = fscanf(logical_address_fp,"%d", &logical_address);
        if (num_matched > 0){
            page = get_page_from(logical_address);
            offset = get_offset_from(logical_address);
            signed char value = get_value(logical_address);
            frame = page_table_lookup(page);
            int physical_address = (frame << 8) | offset;
            printf("(%d,%d) %d, (%d, %d) %d, %d \n", page, offset, logical_address, frame, 
                                offset, physical_address, value);
        }
        is_done = feof(logical_address_fp);
    }
    printf("TLB Hit Rate: %.2f %%\n", get_TLB_hit_rate()*100);
    printf("Page Fault Rate: %.2f %%\n", get_page_fault_rate()*100);
    close_backing_store();
    free_physical_memory();
    close_page_table();
    fclose(logical_address_fp);
}
