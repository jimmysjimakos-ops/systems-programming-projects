#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>


void keyboard_init(void);
void keyboard_handler(void);
uint8_t kbd_getchar(uint8_t *out);

#endif