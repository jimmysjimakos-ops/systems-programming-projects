#include <stdint.h>
#include "idt.h"
 
struct idt_entry idt[256];
struct idt_pointer idt_ptr;

extern uint32_t isr_table[256];

void idt_set_entry(int index, uint32_t handler, uint16_t segment_selector, uint8_t zero, uint8_t type){
    idt[index].handler_low = handler & 0xFFFF; //low bytes are right most  , mask directly
    idt[index].segment_selector = segment_selector;
    idt[index].zero = zero;
    idt[index].type = type; 
    idt[index].handler_high = (handler >> 16) & 0xFFFF;
}

void idt_init(){
    for(int i = 0; i < 48; i++){  //the rest cells are garbage havent implemented their stubs yet
        idt_set_entry(i , isr_table[i] , 0x08 , 0 , 0x8E);
    }
    idt_ptr.base = (uint32_t) idt;
    idt_ptr.limit = sizeof(idt) - 1;
    idt_load();
    
}