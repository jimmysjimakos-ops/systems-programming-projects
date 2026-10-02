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

void task_a(){asm volatile("sti"); while(1){ mutex_lock(&lock); print("A");  mutex_unlock(&lock);  for(volatile int i=0;i<3000000;i++);} }
void task_b(){asm volatile("sti"); while(1){ mutex_lock(&lock); print("B");  mutex_unlock(&lock);  for(volatile int i=0;i<3000000;i++);} }
void task_c(){asm volatile("sti"); while(1){ mutex_lock(&lock); print("C");  mutex_unlock(&lock);  for(volatile int i=0;i<3000000;i++);} }
void task_d(){asm volatile("sti"); while(1){ mutex_lock(&lock); print("D");  mutex_unlock(&lock);  for(volatile int i=0;i<3000000;i++);} }
void task_e(){asm volatile("sti"); while(1){ mutex_lock(&lock); print("E");  mutex_unlock(&lock);  for(volatile int i=0;i<3000000;i++);} }


void idle(){asm volatile("sti"); while(1) asm volatile("hlt"); }

//void task_b(){asm volatile("sti");  while(1){ mutex_lock(&lock); print("B"); mutex_unlock(&lock); for(volatile int i=0;i<3000000;i++); } }
//void task_c(){asm volatile("sti");  while(1){ mutex_lock(&lock); print("C"); mutex_unlock(&lock); for(volatile int i=0;i<3000000;i++); } }
//void task_d(){asm volatile("sti");  while(1){ mutex_lock(&lock); print("D"); mutex_unlock(&lock); for(volatile int i=0;i<3000000;i++); } }
//void task_e(){asm volatile("sti");  while(1){ mutex_lock(&lock); print("E"); mutex_unlock(&lock); for(volatile int i=0;i<3000000;i++); } }



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
    pmm_init(mboot);
    extern uint32_t stack_bottom;   // from boot.asm
    vmm_unmap_page((uint32_t)&stack_bottom - 4096);   // guard below boot stack


    //extern uint32_t _kernel_end;
    //char dbuf1[32];
    //char dbuf2[32];
    //uint32_t *perma_pointer = (uint32_t *) kmalloc(100);
    //*perma_pointer = 42;
    //itoa((uint32_t)perma_pointer, dbuf1);
    //itoa((uint32_t)(*perma_pointer), dbuf2);
    //print(dbuf1);
    //print(dbuf2);
    //volatile uint32_t *bad = (volatile uint32_t *)0x500000; // 5MB - unmapped
    //uint32_t test = *bad;
    mutex_init(&lock);
    create_task(idle, 0);
    create_task(task_a, 1);
    create_task(task_b, 2);
    create_task(task_c, 3);
    create_task(task_d, 4);
    create_task(task_e, 5);
  
    schedule();
    while(1){   
        asm volatile("hlt"); //allows interrupts to happen , halts the execution of kmain only
    }

}

