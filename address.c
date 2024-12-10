#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Configuration Constants
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define MEMORY_SIZE (PAGE_TABLE_SIZE * FRAME_SIZE)
#define BACKING_STORE_PATH "BACKING_STORE.bin"

// Advanced Replacement Strategies
typedef enum {
    FIFO,
    LRU
} ReplacementStrategy;

// Improved TLB Entry Structure
typedef struct {
    int page_number;
    int frame_number;
    time_t last_access;
    int valid;
} TLBEntry;

// Memory Management Control Structure
typedef struct {
    TLBEntry tlb[TLB_SIZE];
    int page_table[PAGE_TABLE_SIZE];
    signed char physical_memory[MEMORY_SIZE];
    
    // Additional tracking
    int tlb_hits;
    int tlb_misses;
    int page_faults;
    
    // Replacement strategy
    ReplacementStrategy strategy;
    int tlb_next_index;
} MemorySystem;

// Function Prototypes
MemorySystem* create_memory_system(ReplacementStrategy strategy);
void destroy_memory_system(MemorySystem* system);
void initialize_memory_system(MemorySystem* system);
int tlb_lookup(MemorySystem* system, int page_number);
void tlb_update(MemorySystem* system, int page_number, int frame_number);
int page_table_lookup(MemorySystem* system, int page_number);
int handle_page_fault(MemorySystem* system, int page_number);
int translate_logical_address(MemorySystem* system, int logical_address, int* physical_address);
signed char read_from_physical_memory(MemorySystem* system, int physical_address);
void write_to_physical_memory(MemorySystem* system, int physical_address, signed char value);
void load_page_from_backing_store(MemorySystem* system, int page_number, int frame_number);

// Create Memory System
MemorySystem* create_memory_system(ReplacementStrategy strategy) {
    MemorySystem* system = malloc(sizeof(MemorySystem));
    if (!system) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    system->strategy = strategy;
    initialize_memory_system(system);
    return system;
}

// Initialize Memory System
void initialize_memory_system(MemorySystem* system) {
    memset(system->tlb, 0, sizeof(system->tlb));
    memset(system->page_table, -1, sizeof(system->page_table));
    memset(system->physical_memory, 0, sizeof(system->physical_memory));
    
    // Reset tracking metrics
    system->tlb_hits = 0;
    system->tlb_misses = 0;
    system->page_faults = 0;
    system->tlb_next_index = 0;
    
    // Initialize TLB entries as invalid
    for (int i = 0; i < TLB_SIZE; i++) {
        system->tlb[i].valid = 0;
    }
}

// TLB Lookup with LRU and FIFO support
int tlb_lookup(MemorySystem* system, int page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (system->tlb[i].valid && system->tlb[i].page_number == page_number) {
            system->tlb_hits++;
            
            // Update last access time for LRU
            if (system->strategy == LRU) {
                system->tlb[i].last_access = time(NULL);
            }
            
            return system->tlb[i].frame_number;
        }
    }
    
    system->tlb_misses++;
    return -1; // TLB miss
}

// TLB Update with Replacement Strategies
void tlb_update(MemorySystem* system, int page_number, int frame_number) {
    int replace_index = system->tlb_next_index;
    
    if (system->strategy == LRU) {
        // Find LRU entry
        time_t oldest_time = system->tlb[0].last_access;
        for (int i = 1; i < TLB_SIZE; i++) {
            if (system->tlb[i].last_access < oldest_time) {
                oldest_time = system->tlb[i].last_access;
                replace_index = i;
            }
        }
    }
    
    // Update TLB entry
    system->tlb[replace_index].page_number = page_number;
    system->tlb[replace_index].frame_number = frame_number;
    system->tlb[replace_index].last_access = time(NULL);
    system->tlb[replace_index].valid = 1;
    
    // Move to next index for FIFO
    system->tlb_next_index = (system->tlb_next_index + 1) % TLB_SIZE;
}

// Page Table Lookup
int page_table_lookup(MemorySystem* system, int page_number) {
    return system->page_table[page_number];
}

// Handle Page Fault with Backing Store Support
int handle_page_fault(MemorySystem* system, int page_number) {
    static int next_frame = 0;
    int frame_number = next_frame++;
    
    if (next_frame >= PAGE_TABLE_SIZE) {
        fprintf(stderr, "Error: Out of physical memory\n");
        exit(1);
    }
    
    system->page_faults++;
    system->page_table[page_number] = frame_number;
    
    // Load page from backing store
    load_page_from_backing_store(system, page_number, frame_number);
    
    return frame_number;
}

// Load Page from Backing Store
void load_page_from_backing_store(MemorySystem* system, int page_number, int frame_number) {
    FILE* backing_store = fopen(BACKING_STORE_PATH, "rb");
    if (!backing_store) {
        perror("Error opening backing store");
        exit(1);
    }
    
    // Seek to the correct page
    if (fseek(backing_store, page_number * FRAME_SIZE, SEEK_SET) != 0) {
        perror("Error seeking in backing store");
        fclose(backing_store);
        exit(1);
    }
    
    // Read page into physical memory
    if (fread(system->physical_memory + (frame_number * FRAME_SIZE), 
              1, FRAME_SIZE, backing_store) != FRAME_SIZE) {
        perror("Error reading from backing store");
        fclose(backing_store);
        exit(1);
    }
    
    fclose(backing_store);
}

// Translate Logical Address
int translate_logical_address(MemorySystem* system, int logical_address, int* physical_address) {
    int page_number = (logical_address >> 8) & 0xFF;
    int offset = logical_address & 0xFF;

    int frame_number = tlb_lookup(system, page_number);

    if (frame_number == -1) { // TLB miss
        frame_number = page_table_lookup(system, page_number);
        
        if (frame_number == -1) { // Page fault
            frame_number = handle_page_fault(system, page_number);
        }
        
        tlb_update(system, page_number, frame_number);
    }

    *physical_address = (frame_number << 8) | offset;
    return frame_number;
}

// Read from Physical Memory
signed char read_from_physical_memory(MemorySystem* system, int physical_address) {
    if (physical_address < 0 || physical_address >= MEMORY_SIZE) {
        fprintf(stderr, "Invalid physical address: %d\n", physical_address);
        return -1;
    }
    return system->physical_memory[physical_address];
}

// Write to Physical Memory
void write_to_physical_memory(MemorySystem* system, int physical_address, signed char value) {
    if (physical_address < 0 || physical_address >= MEMORY_SIZE) {
        fprintf(stderr, "Invalid physical address: %d\n", physical_address);
        return;
    }
    system->physical_memory[physical_address] = value;
}

// Print Memory System Statistics
void print_memory_system_stats(MemorySystem* system) {
    printf("Memory System Statistics:\n");
    printf("TLB Hits: %d\n", system->tlb_hits);
    printf("TLB Misses: %d\n", system->tlb_misses);
    printf("Page Faults: %d\n", system->page_faults);
    printf("TLB Hit Ratio: %.2f%%\n", 
           (system->tlb_hits * 100.0) / (system->tlb_hits + system->tlb_misses));
}

// Example Main Function (for demonstration)
int main() {
    // Create memory system with LRU replacement
    MemorySystem* memory_system = create_memory_system(LRU);
    
    // Example usage would go here
    // Translate addresses, read memory, etc.
    
    // Print statistics before cleanup
    print_memory_system_stats(memory_system);
    
    // Clean up
    destroy_memory_system(memory_system);
    
    return 0;
}

// Cleanup Function
void destroy_memory_system(MemorySystem* system) {
    free(system);
}