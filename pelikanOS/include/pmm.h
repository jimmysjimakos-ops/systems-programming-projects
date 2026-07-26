#include <stdint.h>
#include <mboot.h>


extern uint8_t bitmap[4096];

void pmm_init(struct multiboot_info *mboot);