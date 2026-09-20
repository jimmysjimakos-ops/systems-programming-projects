#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#define TASK_BLOCKED 0
#define TASK_READY   1
#define TASK_RUNNING 2
#define TASK_DEAD    3

typedef struct task{   //this should be encapsulated
    uint32_t ESP;
    uint32_t EIP;
    uint32_t page_directory;
    uint8_t state;
    uint32_t stack_base;
    uint32_t id;
    struct task *next;
    struct task *prev;
    struct task *mutex_next;
}task_t;

task_t *create_task(void (*func)() , int id);
void insert_task(task_t *task);
void remove_task(uint32_t *task);
int get_num_of_tasks();
task_t *get_current_task();
extern void context_switch(task_t *task); 

#endif