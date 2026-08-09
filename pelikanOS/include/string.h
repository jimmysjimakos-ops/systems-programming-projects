#ifndef STRING_H
#define STRING_H

#include <stdint.h>

void memset(void *buf, uint8_t val, uint32_t size);
void memcpy(void *dest, const void *src, uint32_t size);

#endif