#include "keyboard.h"
#include "vga.h"
#include "string.h"

void shell_run(){
    uint8_t linebuf[129];
    uint32_t linebuf_index = 0;
    memset(linebuf, 0 , 129 * sizeof(uint8_t));
    uint8_t c;
    while(1){
        if(kbd_getchar(&c)){
            if(c == '\b'){
                if(linebuf_index > 0){
                    linebuf[--linebuf_index] = '\0';
                    removeChar();
                }
            }else if(c == '\n'){
                // match with dispatch table
                print("\n");
                print(linebuf); //table has been memseted - already contains null terminator
                memset(linebuf, 0 , 129 * sizeof(uint8_t));
                linebuf_index = 0;
                print("\n");
            }else{
                if(linebuf_index < 127){
                    linebuf[linebuf_index++] = c;
                    char s[2] = {c, '\0'};
                    print(s);
                }
            }
        }
    }
}