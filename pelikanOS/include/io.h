#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t byte_value){
    asm volatile ("outb %0, %1" : : "a"(byte_value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port){
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline uint32_t save_flags_and_cli(){
    uint32_t result; 
    asm volatile("pushfl\n\t" "popl %%eax\n\t" "mov %%eax , %0\n\t" "cli" : "=r"(result) :: "eax","memory");
    return result;
}

static inline void restore_flags(uint32_t flags){
    asm volatile("pushl %0\n\t" "popfl" :: "r"(flags) : "memory");
}

#endif