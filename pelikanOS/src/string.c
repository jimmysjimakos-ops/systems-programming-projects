#include "string.h"

void memset(void *buf, uint8_t val, uint32_t size){
    uint8_t *p = (uint8_t *)buf;
    for(uint32_t i = 0; i < size; i++){
        p[i] = val;
    }
}