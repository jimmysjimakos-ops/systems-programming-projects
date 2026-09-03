#include "vga.h"

struct vga_state vga = {0, 0, (volatile uint16_t *)0xB8000};
  
int makeSpace(){ //every vga.row replaces its contents by the next vga.row
    vga.row = 0;
    vga.col = 0;

   
    while(vga.row < 25){
        while(vga.col < 80){
            if(vga.row == 24){
                vga.addr[(vga.row * 80 + vga.col)] = ' ';
            }else{
                vga.addr[(vga.row * 80 + vga.col)] = vga.addr[((vga.row + 1) * 80 + vga.col)]; //23 vga.row gets contents of 24        
            }
            vga.col++;
        }
        vga.row++;
        vga.col = 0;
          
    }
    vga.row = 24; //set to new empty line
    vga.col = 0; 
    return 0;
}

int mapToVGA(char s){
    if(s == '\n' && vga.row != 24){
        vga.col = 0;   
        vga.row++;
        return 0;
    }else if(s == '\n' && vga.row == 24){
        makeSpace();
        return 0;
    }

    if(vga.row < 25){
        if(vga.col < 80){
            vga.addr[(vga.row * 80 + vga.col)] = ((0x0F << 8) | s);
        }
        vga.col++;
        if(vga.col >= 80){
            vga.col = 0; //reset
            vga.row++;
            if(vga.row >= 25){
                makeSpace();
            }
        }
    }
    return 0;  //slight performance hinder i think
}

int print(char *s){
    int i = 0;
    while(s[i] != '\0'){
        mapToVGA(s[i]);
        i++;
    }
    return 0;
}

int removeChar(){
    if(vga.row > 0){
        if(vga.col == 0){
            vga.row--;
            vga.col = 79;
            vga.addr[(vga.row * 80 + vga.col)] = ' ';
        }else if(vga.col > 0){
            vga.col--;
            vga.addr[(vga.row * 80 + vga.col)] = ' ';
        }
    }else{
        if(vga.col > 0){
            vga.col--;
            vga.addr[(vga.row * 80 + vga.col)] = ' ';
        }
    }

    return 0;
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