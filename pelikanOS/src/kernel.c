#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "vga.h"
#include "pic.h"

void kmain(uint32_t magic, void  *mboot_info){
    gdt_init();
    idt_init();
    pic_remap();
    asm volatile("sti"); //enable interrupts
    char s[13] = {'P' , 'E' , 'L' , 'I' , 'K' , 'A' , 'N' , '\n', 'a' , 'K' , 'a', '\n', '\0'};
    int s_len = 12;

    if(printString(s, s_len) > 0){
        //not enough space
    }else{
        vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | '/');
    }

    
    while(1){
    asm volatile("hlt");
    }

}