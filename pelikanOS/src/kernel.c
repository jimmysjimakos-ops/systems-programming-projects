#include <stdint.h>

int row=0;
int col=0;
int screen_space_limit = 1999;
volatile uint16_t *vga_addr = (volatile uint16_t *) 0xB8000;


int mapToVGA(char s , int string_len){
    if(row < 25){
        if(col < 80){
            vga_addr[(row * 80 + col)] = ((0x0F << 8) | s);
            col++;
            if(col >= 80){
                col = 0; //reset
                row++;
                if(row >= 25){
                    //add scrolling?
                    row = 24;
                    col = 79;
                    vga_addr[(row * 80 + col)] = ((0x0F << 8) | '/');
                }
            }
        }
        
    }
    return 0;  //slight performance hinder i think

}
int printString(char *s , int string_len){
    for(int i = 0; i < string_len; i++){
        if(mapToVGA(s[i] , string_len) > 0){
            return 1;
        }
    }
    return 0;
}


void kmain(uint32_t magic, void  *mboot_info){
    char s[8] = {'P' , 'E' , 'L' , 'I' , 'K' , 'A' , 'N' , '\0'};
    int s_len = 83;

    if(printString(s, s_len) > 0){
        //not enough space
    }else{
        vga_addr[(row * 80 + col)] = ((0x0F << 8) | '/');
    }
    return;
}