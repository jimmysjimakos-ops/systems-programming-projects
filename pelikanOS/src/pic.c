#include <stdint.h>
#include "io.h"
#include "pic.h"

void pic_remap(){
    outb(0x20, 0x11); //ICW1 , 
    outb(0xA0, 0x11);
    
    outb(0x21, 0x20); //ICW2 REMAP MASTER
    outb(0xA1, 0x28); //ICW2 REMAP SLAVE

    outb(0x21, 0x04); //ICW3 (SLAVE IS AT IRQ2) 
    outb(0xA1, 0x02); //ICW3 (SLAVE SENDS ID)

    outb(0x21, 0x01); //ICW4 (8086 MODE)
    outb(0xA1, 0x01); //ICW4 (8086 MODE)

    outb(0x21, 0x00); //SET INTERRUPT MASKS TO ZERO
    outb(0xA1, 0x00); //SET INTERRUPT MASKS TO ZERO

}