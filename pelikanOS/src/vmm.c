#include "vmm.h"
#include "pmm.h"
#include "string.h"
#include "paging.h"
#include "itoa.h"
#include "vga.h"
#define MAX_ENTRIES 1024

static uint32_t *page_directory = 0;
static uint32_t *page_table = 0;

void vmm_init(){
    page_directory = (uint32_t *) pmm_alloc_frame();
    memset(page_directory, 0, 4096);
    page_table = (uint32_t *)pmm_alloc_frame();
    for(uint32_t i = 0; i < MAX_ENTRIES; i++){
        page_table[i] = i * 4096 | PAGE_PRESENT | PAGE_RW;
    }
    page_directory[0] = (uint32_t) page_table | PAGE_PRESENT | PAGE_RW;
    char dbuf[32];
    print("VMM: page directory at ");
    itoa((uint32_t)page_directory, dbuf);
    print(dbuf);
    print("\n");
    print("VMM: identity mapped first 4MB\n");
    load_CR3((uint32_t) page_directory);
    print("VMM: CR3 loaded\n");
    flip_bit_31();
    print("VMM: paging enabled\n");
}

void vmm_map_page(uint32_t virt_addr, uint32_t phys_addr){
    uint16_t dir_index = virt_addr >> 22;
    uint16_t table_index = (virt_addr >> 12) & 0x3FF;

    if (!(page_directory[dir_index] & 0x01)) {
        uint32_t *new_table = (uint32_t *)pmm_alloc_frame();
        memset(new_table, 0, 4096);
        page_directory[dir_index] = (uint32_t)new_table | PAGE_PRESENT | PAGE_RW;
        char buf[32];
        print("VMM: new page table for dir ");
        itoa(dir_index, buf);
        print(buf);
        print("\n");
    }

    uint32_t *page_table = (uint32_t *)(page_directory[dir_index] & 0xFFFFF000);
    page_table[table_index] = phys_addr | PAGE_PRESENT | PAGE_RW;

    char vbuf[32], pbuf[32];
    print("VMM: mapped ");
    itoa(virt_addr, vbuf);
    print(vbuf);
    print(" -> ");
    itoa(phys_addr, pbuf);
    print(pbuf);
    print("\n");
}

void vmm_unmap_page(uint32_t virt_addr){
    uint16_t dir_index = virt_addr >> 22;
    uint16_t table_index = (virt_addr >> 12) & 0x3FF;
    uint32_t *page_table = (uint32_t *)(page_directory[dir_index] & 0xFFFFF000);
    page_table[table_index] = 0;
    asm volatile("invlpg (%0)" : : "r"(virt_addr) : "memory");

    char buf[32];
    print("VMM: unmapped ");
    itoa(virt_addr, buf);
    print(buf);
    print("\n");
}


uint32_t vmm_get_kernel_directory(){
    return (uint32_t) page_directory;
}