#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#define PAGE_PRESENT 0x01
#define PAGE_RW 0x02 


void paging_init();
void load_CR3();
void flip_bit_31();

#endif