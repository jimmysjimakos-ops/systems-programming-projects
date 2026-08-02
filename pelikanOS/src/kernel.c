#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "vga.h"
#include "pic.h"
#include "mboot.h"
#include "itoa.h"
#include "pmm.h"
#include "paging.h"

void kmain(uint32_t magic, void  *mboot_info){
    struct multiboot_info *mboot = (struct multiboot_info *) mboot_info;
    char buf[32];
    gdt_init();
    idt_init();
    pic_remap();
    asm volatile("sti"); //enable interrupts
    pmm_init(mboot);
    paging_init();
    uint32_t address = pmm_alloc_frame();
    extern uint32_t _kernel_end;
    char dbuf[32];
    itoa((uint32_t)&_kernel_end, dbuf);
    print("KEND: ");
    print(dbuf);
    print("\n");
    volatile uint32_t *bad = (volatile uint32_t *)0x500000; // 5MB - unmapped
    uint32_t test = *bad;

    vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | '/');
    

    
    while(1){
        asm volatile("hlt");
    }

}