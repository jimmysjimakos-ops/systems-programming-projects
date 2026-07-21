#include <stdint.h>
#include "isr.h" 
#include "vga.h"
#include "io.h" 

char scancode_to_ascii[128] = {
    0, 0, '1', '2', '3', '4', '5', '6',    // 0x00-0x07
    '7', '8', '9', '0', '-', '=', 0, 0,     // 0x08-0x0F (0x0E=backspace, 0x0F=tab)
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', // 0x10-0x17
    'o', 'p', '[', ']', 0, 0, 'a', 's',      // 0x18-0x1F (0x1C=enter, 0x1D=ctrl)
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  // 0x20-0x27
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',  // 0x28-0x2F (0x2A=lshift)
    'b', 'n', 'm', ',', '.', '/', 0, 0,       // 0x30-0x37
    0, ' ', 0,                                 // 0x38=alt, 0x39=space, 0x3A=caps
};
// 0 means uprintable char

void isr_handler(struct interrupt_frame *frame){ //this interrupt runs multiple times , cuz of an IR timer that lives in the hardware , the PIC helps here
    
    uint8_t scancode;

    if(frame->ir_num == 32){
        // timer tick — do nothing for now
    }

    if(frame->ir_num == 33){  //keyboard
        scancode = inb(0x60);
        if(scancode & 0x80){ //check highest(7nth) bit
            //key release - ignore for now
        }else{
            char key = scancode_to_ascii[scancode];
            if(key == 0){  //backspace , alt , shit , caps lock
                //implement later
            }else{
                printString(&key, 1);
            }
        }
    }

    if(frame->ir_num >= 32){
        outb(0x20, 0x20); //EOI TO MASTER
    }
    if(frame->ir_num >=40){
        outb(0xA0, 0x20); //EOI TO SLAVE
    }
}