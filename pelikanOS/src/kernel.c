#include <stdint.h>


void kmain(uint32_t magic, void  *mboot_info){
    volatile uint16_t *vga_addr = (volatile uint16_t *) 0xB8000;  //literally points to an address
    vga_addr[0] = (0x000F << 8) | 'H';

}