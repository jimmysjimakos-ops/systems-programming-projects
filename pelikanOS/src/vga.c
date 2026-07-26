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