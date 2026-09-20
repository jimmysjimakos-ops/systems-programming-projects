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
#include "mutex_lock.h"

mutex_t lock;
int k = 0;
int attempts = 0;

void task_a(){ asm volatile("sti"); while(1){ mutex_lock(&lock); print("A"); mutex_unlock(&lock); for(volatile int i=0;i<3000000;i++); } }
void task_b(){ asm volatile("sti"); while(1){ mutex_lock(&lock); k++; mutex_unlock(&lock); } }
void task_c(){ asm volatile("sti"); while(1){ mutex_lock(&lock); k++; mutex_unlock(&lock); } }
void task_d(){
    asm volatile("sti");
    while(1){
        mutex_lock(&lock);
        for(volatile int j=0;j<50000;j++);   // long critical section, no print
        k++;
        mutex_unlock(&lock);
    }
}
void task_e(){
    asm volatile("sti");
    while(1){
        mutex_lock(&lock);
        for(volatile int j=0;j<50000;j++);   // long critical section, no print
        k++;
        mutex_unlock(&lock);
    }
}

void idle(){ asm volatile("sti"); while(1) asm volatile("hlt"); }

void monitor(){
    while(1){
        task_t *t = get_current_task();
        for(int i = 0; i < 6; i++){
            ((volatile uint16_t*)0xB8000)[160 + i*4]     = (0x0F<<8)|('0'+t->id);
            ((volatile uint16_t*)0xB8000)[160 + i*4 + 1] = (0x0F<<8)|('0'+t->state);
            ((volatile uint16_t*)0xB8000)[160 + i*4 + 2] = (0x0F<<8)|('>');
            ((volatile uint16_t*)0xB8000)[160 + i*4 + 3] = (0x0F<<8)|('0'+t->next->id);
            t = t->next;
        }
        ((volatile uint16_t*)0xB8000)[185] = (0x0F<<8)|('0'+lock.locked);
        ((volatile uint16_t*)0xB8000)[186] = (0x0F<<8)|('0'+(lock.holder ? lock.holder->id : 9));
        k++;
        char buf[16];
        itoa(k, buf);
        
        for(int i = 0; i < 16 && buf[i]; i++){
            ((volatile uint16_t*)0xB8000)[240 + i] = (0x0F<<8)|buf[i];
        }

        for(volatile int i=0;i<500000;i++);
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
    pit_init(1000);
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
    //print(dbuf1);
    //print(dbuf2);
    //volatile uint32_t *bad = (volatile uint32_t *)0x500000; // 5MB - unmapped
    //uint32_t test = *bad;
    vga.row=0;
    vga.col=0;
    mutex_init(&lock);
    create_task(idle, 0);
    create_task(monitor, 6);
    create_task(task_a, 1);
    create_task(task_b, 2);
    create_task(task_c, 3);
    create_task(task_d, 4);
    create_task(task_e, 5);

    
    while(1){   
        asm volatile("hlt"); //allows interrupts to happen , halts the execution of kmain only
    }

}