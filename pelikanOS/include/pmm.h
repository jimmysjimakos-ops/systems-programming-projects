
#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include "mboot.h"

extern uint8_t bitmap[4096];

void pmm_init(struct multiboot_info *mboot);
uint32_t pmm_alloc_frame();
void pmm_free_frame(uint32_t frame_addr);

#endif