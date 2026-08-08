#include "paging.h"

void load_CR3(uint32_t page_directory){
    asm volatile("mov %0, %%cr3" : : "r"(page_directory)); //move the page_directory pointer to cr3
}

void flip_bit_31(){
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0)); //place the cr0 value in our cr0 C variable
    cr0 |= 0x80000000; // this address sets bit 31 to 1 by OR-ing
    asm volatile("mov %0, %%cr0" : : "r"(cr0)); //place the updated cr0 value back into the cr0 register
}
