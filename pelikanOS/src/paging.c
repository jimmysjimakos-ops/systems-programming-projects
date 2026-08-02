#include "paging.h"
#include "pmm.h"
#include "string.h"
#define MAX_ENTRIES 1024

static uint32_t *page_directory = 0; //make two pointers to store page_tables and physical addresses
static uint32_t *page_table = 0;



void load_CR3(){
    asm volatile("mov %0, %%cr3" : : "r"(page_directory)); //move the page_directory pointer to cr3
}

void flip_bit_31(){
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0)); //place the cr0 value in our cr0 C variable
    cr0 |= 0x80000000; // this address sets bit 31 to 1 by OR-ing
    asm volatile("mov %0, %%cr0" : : "r"(cr0)); //place the updated cr0 value back into the cr0 register
}


void paging_init(){
    page_directory =(uint32_t *) pmm_alloc_frame(); //needa add if checks
    memset(page_directory, 0, sizeof(page_directory) * MAX_ENTRIES);  //0 initialize 
    page_table = (uint32_t *)pmm_alloc_frame(); //needa add if checks
    for(uint32_t i = 0; i < MAX_ENTRIES; i++){  //for each uint32 addr(1024 * 4 = 4096 bytes)
        page_table[i] = i * 4096 | PAGE_PRESENT | PAGE_RW; //physical address
    }
    page_directory[0] = (uint32_t) page_table | PAGE_PRESENT | PAGE_RW; //cast to number so it can be OR-ed
    char dbuf[32];
    itoa((uint32_t)page_directory, dbuf);
    print("PD: ");
    print(dbuf);
    print("\n");
    load_CR3();
    flip_bit_31();
}