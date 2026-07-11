#include <stdint.h>
#include "gdt.h"
 
struct gdt_entry gdt[3];
struct gdt_pointer gdt_ptr;

void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
    gdt[index].base_low = base & 0xFFFF; //low bytes are right most  , mask directly
    gdt[index].base_middle = (base >> 16) & 0x00FF; //shift 16bit to leave middle byte then extract it 
    gdt[index].base_high = (base >> 24) & 0x00FF; //shift 24 bits , leave high byte alone - extract it
    gdt[index].limit = limit & 0xFFFF;
    gdt[index].access = access;
    gdt[index].granularity = ((flags & 0x0F) << 4) | ((limit >> 16) & 0x0F);
}

void gdt_init() {
    gdt_set_entry(0, 0, 0, 0, 0);              
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0x0C);  
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0x0C);  
    
    gdt_ptr.base = (uint32_t)gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_load();    
}