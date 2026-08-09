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

/*

uint8_t *print_hex(uint8_t *buf,uint32_t num){ //works for both byte AND uint
    uint32_t nibble;
    uint32_t and_thing = 0x0000000F;

    for(int i=0; i<8; i++){
        nibble = num & (and_thing << i * 4);
        nibble = nibble >> i;
        if(nibble <= 9){   //must not receive negative nums
            buf[i] = '0' + nibble;
        }else if(nibble > 9){
            buf[i] = (nibble - 10) + 'A';
        }
        
    }
    buf[8] = '\0';

    return buf;
}
    the issue is this prints it reversed , so ill have to reverse it , i can instead work top down
*/

uint8_t *print_hex(uint8_t *buf,uint32_t num){ //works for both byte AND uint
    uint32_t nibble;
    for(int i = 7; i > -1; i--){
        nibble = (num >> (i * 4)) & 0x0F;
         if(nibble <= 9){   //must not receive negative nums
            buf[7-i] = '0' + nibble;
        }else if(nibble > 9){
            buf[7-i] = (nibble - 10) + 'A';
        }
    }
    buf[8] = '\0';

    return buf;
}
