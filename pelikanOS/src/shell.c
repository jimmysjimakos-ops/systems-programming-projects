#include "keyboard.h"
#include "vga.h"
#include "string.h"
#include "itoa.h"
#include "scheduler.h"

typedef struct command{
    char *name;
    void (*fn)(void);
}command_t;

static void cmd_help(void);
static void cmd_clear(void);
static void cmd_ps(void);

static command_t commands[] = {{"help" , cmd_help} , {"clear" , cmd_clear} , {"ps" , cmd_ps}};

#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

static void cmd_help(void){
    print("commands:\n");
    for(uint32_t i = 0; i < NUM_COMMANDS; i++){
        print("  ");
        print(commands[i].name);
        print("\n");
    }
    print("\n");
}

static void cmd_clear(void){
    for(int i = 0; i < 80 * 25; i++){
        removeChar();
    }
}


static void cmd_ps(void){
    scheduler_list_tasks();
}

static void dispatch(const char *line){ //needs to be char for strcmp to work
    if(line[0] == '\0'){
        return;
    }
    for(uint32_t i = 0; i < NUM_COMMANDS; i++){
        if(strcmp(line , commands[i].name) == 0){
            commands[i].fn();
            return;
        }
    }
    print("unknown command : ");
    print(line);
    print("\n");
} 

void shell_run(){
    asm volatile("sti");
    char linebuf[129];
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
                dispatch(linebuf);
                memset(linebuf, 0 , 129 * sizeof(uint8_t));
                linebuf_index = 0;
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