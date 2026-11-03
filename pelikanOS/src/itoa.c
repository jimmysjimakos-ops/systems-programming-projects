#include "itoa.h"

void itoa(uint32_t num , char *buf){
    int dindex = 0;
    int digit;
    while(1){
        digit = num % 10;
        buf[dindex++] = '0' + digit;
        num = num / 10;
        if(num == 0){
            break;
        }
    }

    buf[dindex] = '\0';
    int left = 0;
    int right = dindex-1;
    char temp;
    while(left < right){
        temp = buf[left];
        buf[left] = buf[right];
        buf[right] = temp;

        left++;
        right--;
    }
    
    return;
}

int atoi(const char *s){
    int result = 0;
    int i = 0;
    while(s[i] >= '0' && s[i] <= '9'){
        result = result * 10 + (s[i] - '0');
        i++;
    }
    return result;
}