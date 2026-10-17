#include <stdint.h>
#include "isr.h" 
#include "vga.h"
#include "io.h" 
#include "scheduler.h"
#include "keyboard.h"

volatile uint32_t ticks = 0;

void isr_handler(struct interrupt_frame *frame){ //this interrupt runs multiple times , cuz of an IR timer that lives in the hardware , the PIC helps here
    
    if(frame->ir_num == 32){
        ticks++;
        outb(0x20, 0x20); // EOI once, before schedule
        yield();
        return;
    }

    if(frame->ir_num == 33){
        keyboard_handler();      // all keyboard logic lives in keyboard.c now
        outb(0x20, 0x20);        // EOI
        return;
    }


    if(frame->ir_num == 14){
        uint32_t cr2;
        asm volatile("mov %%cr2, %0" : "=r"(cr2));
        print("PAGE FAULT\n");
        char buf[32];
        print("addr: "); itoa(cr2, buf); print(buf); print("\n");
        print("eip: "); itoa(frame->eip, buf); print(buf); print("\n");
        print("err: "); itoa(frame->err_code, buf); print(buf); print("\n");
        while(1) asm volatile("hlt");
    }
}