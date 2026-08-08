#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "vga.h"
#include "pic.h"
#include "mboot.h"
#include "itoa.h"
#include "pmm.h"
#include "paging.h"
#include "heap.h"
#include "vmm.h"

void kmain(uint32_t magic, void  *mboot_info){
    struct multiboot_info *mboot = (struct multiboot_info *) mboot_info;
    gdt_init();
    idt_init();
    pic_remap();
    asm volatile("sti"); //enable interrupts
    pmm_init(mboot);
    vmm_init();
    extern uint32_t _kernel_end;
    char dbuf1[32];
    char dbuf2[32];
    uint32_t *perma_pointer = (uint32_t *) kmalloc(100);
    *perma_pointer = 42;
    itoa((uint32_t)perma_pointer, dbuf1);
    itoa((uint32_t)(*perma_pointer), dbuf2);
    print(dbuf1);
    print(dbuf2);
    //volatile uint32_t *bad = (volatile uint32_t *)0x500000; // 5MB - unmapped
    //uint32_t test = *bad;

    vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | '/');
    

    
    while(1){
        asm volatile("hlt");
    }

}