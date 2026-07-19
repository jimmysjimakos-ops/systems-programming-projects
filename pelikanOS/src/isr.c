#include <stdint.h>
#include "isr.h" 
#include "vga.h"
#include "io.h" 

void isr_handler(struct interrupt_frame *frame){ //this interrupt runs multiple times , cuz of an IR timer that lives in the hardware , the PIC helps here
    
    char e = 'e';
    char *test_char = &e;

    if(frame->ir_num == 32){
        // timer tick — do nothing for now
    } else {
        printString(test_char, 1);
    }

    if(frame->ir_num >= 32){
        outb(0x20, 0x20); //EOI TO MASTER
    }
    if(frame->ir_num >=40){
        outb(0x0A, 0x20); //EOI TO SLAVE
    }
}