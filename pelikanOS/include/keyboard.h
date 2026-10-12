#include <stdint.h>


void keyboard_init(void);
void keyboard_handler(void);
uint8_t kbd_getchar(uint8_t *out);