#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "vga.h"
#include "pic.h"
#include "mboot.h"
#include "itoa.h"
#include "pmm.h"
#include "paging.h"
#include "heap.h"
#include "vmm.h"
#include "io.h"
#include "scheduler.h"



void task_a(){
    while(1){
        print("A");
        for(volatile int i = 0; i < 10000000; i++);
    }
}

void task_b(){
    while(1){
        print("B");
        for(volatile int i = 0; i < 10000000; i++);
    }
}

void throwaway(){
    while(1){
        print("C");
        for(volatile int i = 0; i < 10000000; i++);
    }
}

void pit_init(uint32_t freq){
    uint32_t divisor = 1193182 / freq;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void kmain(uint32_t magic, void  *mboot_info){
    struct multiboot_info *mboot = (struct multiboot_info *) mboot_info;
    gdt_init();
    idt_init();
    pic_remap();
    //pit_init(2);
    asm volatile("sti"); //enable interrupts
    pmm_init(mboot);
    vmm_init();
    extern uint32_t _kernel_end;
    char dbuf1[32];
    char dbuf2[32];
    uint32_t *perma_pointer = (uint32_t *) kmalloc(100);
    *perma_pointer = 42;
    itoa((uint32_t)perma_pointer, dbuf1);
    itoa((uint32_t)(*perma_pointer), dbuf2);
    print(dbuf1);
    print(dbuf2);
    //volatile uint32_t *bad = (volatile uint32_t *)0x500000; // 5MB - unmapped
    //uint32_t test = *bad;

    vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | '/');
    create_task(throwaway);
    create_task(task_a);
    create_task(task_b);

    
    while(1){   
        asm volatile("hlt"); //allows interrupts to happen , halts the execution of kmain only
    }

}