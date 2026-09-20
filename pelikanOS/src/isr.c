#include <stdint.h>
#include "isr.h" 
#include "vga.h"
#include "io.h" 
#include "scheduler.h"
#define backspace -1
#define enter -2
char scancode_to_ascii[128] = {
    0, 0, '1', '2', '3', '4', '5', '6',    // 0x00-0x07
    '7', '8', '9', '0', '-', '=', backspace, 0,     // 0x08-0x0F (0x0E=backspace, 0x0F=tab)
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', // 0x10-0x17
    'o', 'p', '[', ']', enter, 0, 'a', 's',      // 0x18-0x1F (0x1C=enter, 0x1D=ctrl)
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  // 0x20-0x27
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',  // 0x28-0x2F (0x2A=lshift)
    'b', 'n', 'm', ',', '.', '/', 0, 0,       // 0x30-0x37
    0, ' ', 0,                                 // 0x38=alt, 0x39=space, 0x3A=caps
};
// 0 means uprintable char

void isr_handler(struct interrupt_frame *frame){ //this interrupt runs multiple times , cuz of an IR timer that lives in the hardware , the PIC helps here
    
    uint8_t scancode;

    if(frame->ir_num == 32){
        outb(0x20, 0x20); // EOI once, before schedule
        schedule();
        return;
    }

    if(frame->ir_num == 33){  //keyboard
        scancode = inb(0x60);
        if(scancode & 0x80){ //check highest(7nth) bit
            //key release - ignore for now
        }else{
            char key = scancode_to_ascii[scancode]; //on some archs like arm char is unsigned and this would break the comparisons - here its ok
            if(key == backspace){  //backspace , alt , shit , caps lock
                removeChar();
            }else if(key == enter){
                char buf2[2] = {'\n','\0'};
                print(buf2);
            }else{
                char buf2[2] = {key, '\0'};
                print(buf2);
            }
        }
        outb(0x20, 0x20);   // EOI for keyboard
        return;
    }


    if(frame->ir_num == 14){
        char buf[2]={'n','\0'};
        print(buf);
        while(1) asm volatile("hlt");  // halt — can't recover
    }
}