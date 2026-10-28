#include <stdint.h>
#include "isr.h" 
#include "vga.h"
#include "io.h" 
#include "scheduler.h"
#include "keyboard.h"


static const char *exception_names[] = {
    "Divide by zero",      // 0
    "Debug",               // 1
    "NMI",                 // 2
    "Breakpoint",          // 3
    "Overflow",            // 4
    "Bound range",         // 5
    "Invalid opcode",      // 6
    "Device not available",// 7
    "Double fault",        // 8
    "Coprocessor overrun", // 9
    "Invalid TSS",         // 10
    "Segment not present", // 11
    "Stack-segment fault", // 12
    "General protection",  // 13
    "Page fault",          // 14
    // 15+ reserved/other
};

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

    if(frame->ir_num < 32){
        print("EXCEPTION: ");
        if(frame->ir_num < 15) print(exception_names[frame->ir_num]);
        else print("reserved/unknown");
        print("\n");
        char buf[32];
        print("vec: "); itoa(frame->ir_num, buf); print(buf); print("\n");
        print("eip: "); itoa(frame->eip, buf); print(buf); print("\n");
        print("err: "); itoa(frame->err_code, buf); print(buf); print("\n");
        while(1) asm volatile("hlt");
    }
}