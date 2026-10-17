#ifndef ISR_H
#define ISR_H
#include <stdint.h>

extern volatile uint32_t ticks;

struct interrupt_frame{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ir_num;
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

void isr_handler(struct interrupt_frame *frame);    

#endif