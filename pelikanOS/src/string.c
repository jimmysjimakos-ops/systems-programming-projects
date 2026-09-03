#include "string.h"

void memset(void *buf, uint8_t val, uint32_t size){
    uint8_t *p = (uint8_t *)buf;
    for(uint32_t i = 0; i < size; i++){
        p[i] = val;
    }
}

void memcpy(void *dest, const void *src, uint32_t size){
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for(uint32_t i = 0; i < size; i++){
        d[i] = s[i];
    }
}


