#include <stdint.h>

struct idt_entry{
    uint16_t handler_low;
    uint16_t segment_selector;
    uint8_t  zero;
    uint8_t  type; 
    uint16_t handler_high;
}__attribute__((packed));

struct idt_pointer{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));

extern struct idt_entry idt[256];
extern struct idt_pointer idt_ptr;

void  idt_set_entry(int index, uint32_t handler, uint16_t segment_selector, uint8_t zero, uint8_t type);
void  idt_init(); 

extern void idt_load();
 