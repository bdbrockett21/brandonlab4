#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lab4.h"

#ifndef LAB4_H
#define LAB4_H

#define NUM_PAGES 256
#define TLB_SIZE 16
#define MAIN_MEMORY_SIZE 65536
#define PAGE_SIZE 3


typedef struct {
    int page_number;
    int frame_number;
    int valid;
} TLB_Entry;

typedef struct {
    TLB_Entry* entries;
    int next_index;
} TLB_Table;

typedef struct {
    int frame_number;
} Page_Entry;

typedef struct {
    char* memory;
} Main_Memory;

#endif // LAB4_H


Page_Entry* page_table;
TLB_Table tlb;
Main_Memory main_mem;
FILE* backing_store_fp;


// Implement get_page_from() and get_offset_from() functions below this line
unsigned char get_page_from(int logical_address) {
    return (unsigned char)(logical_address / PAGE_SIZE);
}

unsigned char get_offset_from(int logical_address) {
    return (unsigned char)(logical_address % PAGE_SIZE);
}

// Implement the page table functions below this line
void page_table_init() {
    page_table = (Page_Entry*) malloc(sizeof(Page_Entry) * NUM_PAGES);
    if (page_table == NULL) {
        perror("Memory allocation failed for the page table");
        exit(1);
    }
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i].frame_number = -1;
    }
}

Page_Entry* page_table_get_entry(int page_number) {
    if (page_number < 0 || page_number >= NUM_PAGES) {
        fprintf(stderr, "Invalid page number: %d\n", page_number);
        return NULL;
    }
    return &page_table[page_number];
}

void page_table_set_entry(int page_number, int frame_number) {
    if (page_number < 0 || page_number >= NUM_PAGES) {
        fprintf(stderr, "Invalid page number: %d\n", page_number);
        return;
    }
    page_table[page_number].frame_number = frame_number;
}

// Implement the TLB functions below this line
void tlb_init() {
    tlb.entries = (TLB_Entry*) malloc(sizeof(TLB_Entry) * TLB_SIZE);
    if (tlb.entries == NULL) {
        perror("Memory allocation failed for TLB");
        exit(1);
    }
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb.entries[i].valid = 0;
    }
}

short tlb_lookup(unsigned char page) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb.entries[i].valid && tlb.entries[i].page_number == page) {
            return (short)tlb.entries[i].frame_number;
        }
    }
    return -1;
}

void tlb_insert(int page_number, int frame_number) {
    int oldest_index = tlb.next_index;
    tlb.entries[oldest_index].page_number = page_number;
    tlb.entries[oldest_index].frame_number = frame_number;
    tlb.entries[oldest_index].valid = 1;
    tlb.next_index = (tlb.next_index + 1) % TLB_SIZE;
}


// Implement the Physical Memory functions below this line
void main_memory_init() {
  main_mem.memory = (char*) malloc(sizeof(char) * MAIN_MEMORY_SIZE);
  if (main_mem.memory == NULL) {
    perror("Memory allocation failed for main memory");
    exit(1);
  }
  for (int i = 0; i < MAIN_MEMORY_SIZE; i++) {
    main_mem.memory[i] = 0;
  }
}
void main_memory_read(int frame_number, int offset, char* buffer) {
  int physical_address = (frame_number * PAGE_SIZE) + offset;
  if (physical_address < 0 || physical_address >= MAIN_MEMORY_SIZE) {
    fprintf(stderr, "Invalid physical address: %d\n", physical_address);
    return;
  }

  *buffer = main_mem.memory[physical_address];
}
void main_memory_write(int frame_number, int offset, char* buffer) {
  int physical_address = (frame_number * PAGE_SIZE) + offset;
    if (physical_address < 0 || physical_address >= MAIN_MEMORY_SIZE) {
    fprintf(stderr, "Invalid physical address: %d\n", physical_address);
    return;
  }
  main_mem.memory[physical_address] = *buffer; 
}


// Implement the Backing Store functions below this line
void backing_store_init(char* file_name) {
  backing_store_fp = fopen(file_name, "r+");
  if (backing_store_fp == NULL) {
    perror("Error opening backing store file");
    exit(1);
  }
}
void backing_store_read(int page_number, char* buffer) {
  long int offset = (long int) page_number * PAGE_SIZE;
  if (fseek(backing_store_fp, offset, SEEK_SET) != 0) {
    perror("Error seeking in backing store file");
    exit(1);
  }
 
  if (fread(buffer, sizeof(char), PAGE_SIZE, backing_store_fp) != PAGE_SIZE) {
    perror("Error reading from backing store file");
    exit(1);
  }
}
void backing_store_write(int page_number, char* buffer) {
  long int offset = (long int) page_number * PAGE_SIZE;
  if (fseek(backing_store_fp, offset, SEEK_SET) != 0) {
    perror("Error seeking in backing store file");
    exit(1);
  }
  if (fwrite(buffer, sizeof(char), PAGE_SIZE, backing_store_fp) != PAGE_SIZE) {
    perror("Error writing to backing store file");
    exit(1);
  }
}
