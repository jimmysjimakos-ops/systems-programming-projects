#include "keyboard.h"
#include "vga.h"
#include "string.h"
#include "itoa.h"
#include "scheduler.h"
#include "pmm.h"

typedef struct command{
    char *name;
    void (*fn)(void);
}command_t;

typedef struct spawnable{
    char *name;
    void (*fn)(void);
}spawnable_t;

static void worker_counter(void){
    asm volatile("sti");
    volatile uint32_t n = 0;
    while(1){
        n++;
        for(volatile int i = 0; i < 3000000; i++);
    }
}

static void worker_once(void){
    asm volatile("sti");
    print("[worker ran once]\n");
    thread_exit();
}

static spawnable_t spawnables[] = {
    {"counter", worker_counter},
    {"once",    worker_once},
};
#define NUM_SPAWNABLES (sizeof(spawnables) / sizeof(spawnables[0]))

static char spawn_arg[129]; //spawn func's argument
static int next_spawn_id = 100;   // ids for spawned tasks sp they avoid clashing with fixed ones

static void cmd_help(void);
static void cmd_clear(void);
static void cmd_ps(void);
static void cmd_mem(void);
static void cmd_spawn(void);

static command_t commands[] = {
    {"help" , cmd_help},
    {"clear", cmd_clear},
    {"ps"   , cmd_ps},
    {"mem"  , cmd_mem},
    {"spawn", cmd_spawn},
};
#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

static void cmd_help(void){
    print("commands:\n");
    for(uint32_t i = 0; i < NUM_COMMANDS; i++){
        print("  ");
        print(commands[i].name);
        print("\n");
    }
    print("spawnable: ");
    for(uint32_t i = 0; i < NUM_SPAWNABLES; i++){
        print(spawnables[i].name);
        print(" ");
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

static void cmd_mem(void){
    char buf[32];
    itoa(pmm_free_count(), buf);
    print("free frames: ");
    print(buf);
    print("\n");
}

static void cmd_spawn(void){
    if(spawn_arg[0] == '\0'){
        print("usage: spawn <name>\n");
        return;
    }
    for(uint32_t i = 0; i < NUM_SPAWNABLES; i++){
        if(strcmp(spawn_arg, spawnables[i].name) == 0){
            create_task(spawnables[i].fn, next_spawn_id, 0);
            char buf[32];
            print("spawned '");
            print(spawn_arg);
            print("' as id ");
            itoa(next_spawn_id, buf);
            print(buf);
            print("\n");
            next_spawn_id++;
            return;
        }
    }
    print("no such task: ");
    print(spawn_arg);
    print("\n");
}

static void dispatch(const char *line){
    if(line[0] == '\0'){
        return;
    }

    char cmd[129];
    uint32_t i = 0;
    while(line[i] != '\0' && line[i] != ' ' && i < 128){
        cmd[i] = line[i];
        i++;
    }
    cmd[i] = '\0';

    // skip the space(s), copy the rest into spawn_arg
    uint32_t j = 0;
    if(line[i] == ' '){
        i++;   // skip the single separating space
        while(line[i] != '\0' && j < 128){
            spawn_arg[j++] = line[i++];
        }
    }
    spawn_arg[j] = '\0'; // this and the starting at 0 index on each dispatch call ensures that spawn_arg is never REUSED WITH THE SAME CONTENTS 

    for(uint32_t k = 0; k < NUM_COMMANDS; k++){
        if(strcmp(cmd, commands[k].name) == 0){
            commands[k].fn();
            return;
        }
    }
    print("unknown command : ");
    print(cmd);
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