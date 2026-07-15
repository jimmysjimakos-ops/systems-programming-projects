#include <stdint.h>
#include "isr.h" 
#include "vga.h"

void isr_handler(struct interrupt_frame *frame){ //this interrupt runs multiple times , cuz of an IR timer that lives in the hardware , the PIC helps here
    volatile uint16_t *v = (volatile uint16_t *)0xB8000;
        v[0] = (0x0F << 8) | 'E';
}