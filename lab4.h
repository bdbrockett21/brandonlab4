#define FRAME_ADDR_BITS  8
#define PAGE_ADDR_BITS  8
#define OFFSET_ADDR_BITS 8
#define TLB_LEN 16

typedef struct _page_entry {
    unsigned char frame;
    char is_valid;
} Page_Entry;

typedef struct _tlb_entry {
    unsigned char page;
    unsigned char frame;
    char is_valid;
}TLB_Entry;

typedef struct _tlb_table{
    TLB_Entry* entry;
    int length;
    int head;
    int tail;
    unsigned char is_full;
}TLB_Table;

typedef struct _main_memory{
    char* mem;
    int next_available_frame;
} Main_Memory;

unsigned char get_page_from(int logical_address);
unsigned char get_offset_from(int logical_address);

// page table functions
void init_page_table();
void close_page_table();
short page_table_lookup(unsigned char);
void update_page_table(unsigned char page, unsigned char frame);

//TLB functions
void init_tlb();
short tlb_lookup(unsigned char page);
void update_tlb(unsigned char page, unsigned char frame);
void close_tlb();

// Physical memory functions
void init_physical_memory();
void free_physical_memory();
// Returns the signed value stored at the logical_address
signed char get_value(int logical_address);

// Backing store functions
void init_backing_store(char* filename);
int roll_in(unsigned char page, unsigned char frame);
void close_backing_store();

// Stats functions
double get_TLB_hit_rate();
double get_page_fault_rate();
void increment_tlb_hit();
void increment_page_fault_count();
void increment_total_page_request();
