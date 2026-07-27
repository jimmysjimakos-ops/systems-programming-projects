#ifndef MBOOT_H
#define MBOOT_H

#include <stdint.h>

struct multiboot_info{
    uint32_t flags;
    uint32_t mem_lower; // KB of memory below 1MB
    uint32_t mem_upper; // KB of memory above 1MB
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
};

struct multiboot_mmap_entry{
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type; // 1 = available , everything else = reserved 
};

#endif