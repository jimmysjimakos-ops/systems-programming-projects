#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "vga.h"


void kmain(uint32_t magic, void  *mboot_info){
    idt_init();
    gdt_init();
    char s[13] = {'P' , 'E' , 'L' , 'I' , 'K' , 'A' , 'N' , '\n', 'a' , 'K' , 'a', '\n', '\0'};
    int s_len = 12;

    if(printString(s, s_len) > 0){
        //not enough space
    }else{
        vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | '/');
    }

    asm volatile ("int $0x03"); //manual fire of an interrupt
    asm volatile ("int $0x03"); //manual fire of an interrupt

    return;
}