#include <stdint.h>

int row=0;
int col=0;
int screen_space_limit = 1999;
volatile uint16_t *vga_addr = (volatile uint16_t *) 0xB8000;

int makeSpace(){ //every row replaces its contents by the next row
    row = 0;
    col = 0;

   
    while(row < 25){
        while(col < 80){
            if(row == 24){
                vga_addr[(row * 80 + col)] = ' ';
            }else{
                vga_addr[(row * 80 + col)] = vga_addr[((row + 1) * 80 + col)]; //23 row gets contents of 24        
            }
            col++;
        }
        row++;
        col = 0;
          
    }
    row = 24; //set to new empty line
    col = 0; 
    return 0;
}

int mapToVGA(char s , int string_len){
    if(s == '\n' && row != 24){
        col = 0;   
        row++;
        return 0;
    }else if(s == '\n' && row == 24){
        makeSpace();
        return 0;
    }

    if(row < 25){
        if(col < 80){
            vga_addr[(row * 80 + col)] = ((0x0F << 8) | s);
        }
        col++;
        if(col >= 80){
            col = 0; //reset
            row++;
            if(row >= 25){
                makeSpace();
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
    char s[12] = {'P' , 'E' , 'L' , 'I' , 'K' , 'A' , 'N' , '\n', 'a' , 'K' , 'a', '\0'};
    int s_len = 2000;

    if(printString(s, s_len) > 0){
        //not enough space
    }else{
        vga_addr[(row * 80 + col)] = ((0x0F << 8) | '/');
    }
    return;
}