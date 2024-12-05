#include <stdio.h>
#include "lab4.h"

int tlb_hit_count = 0;
int page_fault_count = 0;
int total_page_requests = 0;

void increment_tlb_hit(){
    tlb_hit_count++;
}

void increment_total_page_request(){
    total_page_requests++;
}

void increment_page_fault_count(){
    page_fault_count++;
}

double get_TLB_hit_rate(){
    return (double)(tlb_hit_count)/total_page_requests;
}

double get_page_fault_rate(){
    return (double)(page_fault_count)/total_page_requests;
}
