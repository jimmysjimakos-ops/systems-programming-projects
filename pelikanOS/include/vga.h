#include <stdint.h>

struct vga_state{
    int row;
    int col;
    volatile uint16_t *addr;
};

extern struct vga_state vga;

int makeSpace(); 
int mapToVGA(char s , int string_len);
int printString(char *s , int string_len);