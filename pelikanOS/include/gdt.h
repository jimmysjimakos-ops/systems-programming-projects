#include <stdint.h>

struct gdt_entry{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));

struct gdt_pointer{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));

extern struct gdt_entry gdt[3]; //doing it only for include purposes
extern struct gdt_pointer gdt_ptr;

void gdt_init();

extern void gdt_load();