#ifndef VMM_H
#define VMM_H
#include <stdint.h>
#define PAGE_PRESENT 0x01
#define PAGE_RW 0x02 

void vmm_init();
void vmm_map_page(uint32_t virt_addr, uint32_t phys);
void vmm_unmap_page(uint32_t virt_addr);
uint32_t vmm_get_kernel_directory();

#endif