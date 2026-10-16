#include <stdint.h>

struct vga_state{
    int row;
    int col;
    volatile uint16_t *addr;
};

extern struct vga_state vga;

int makeSpace(); 
int mapToVGA(const char s);
int print(const char *s);
int removeChar();
uint8_t *print_hex(uint8_t *buf,uint32_t num);
