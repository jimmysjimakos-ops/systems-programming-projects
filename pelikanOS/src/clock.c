#include "clock.h"

void clock_thread(void){
    asm volatile("sti");
    char buf[16];
    while(1){
        itoa(ticks, buf);
        print_at(0, 68, "ticks:        ");  // clear old digits first
        print_at(0, 74, buf);               // then draw current
        for(volatile int i = 0; i < 2000000; i++);  // crude redraw delay
    }
}