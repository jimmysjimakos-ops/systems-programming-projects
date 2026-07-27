#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "vga.h"
#include "pic.h"
#include "mboot.h"
#include "itoa.h"
#include "pmm.h"

void kmain(uint32_t magic, void  *mboot_info){
    struct multiboot_info *mboot = (struct multiboot_info *) mboot_info;
    char buf[32];
    gdt_init();
    idt_init();
    pic_remap();
    asm volatile("sti"); //enable interrupts
    pmm_init(mboot);
    uint32_t address = pmm_alloc_frame();
    itoa(address, buf);
    print(buf);

    vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | '/');
    

    
    while(1){
    asm volatile("hlt");
    }

}