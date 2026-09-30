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
    struct task *next; //purely cosmetic at this point
    struct task *prev; //purely cosmetic at this point
    struct task *ready_next;
    struct task *mutex_wait_next;
}task_t;

typedef struct ready_queue{
    task_t *queue_head;
    task_t *queue_tail;
}rd_q;

uint8_t isempty_ready_queue(rd_q *q);
void add_task_to_ready_queue(rd_q *q , task_t *task);
task_t *remove_task_from_ready_queue(rd_q *q);
task_t *create_task(void (*func)() , int id);
void insert_task(task_t *task);
void remove_task(uint32_t *task);
int get_num_of_tasks();
task_t *get_current_task();
//extern void context_switch(task_t *task); 
extern rd_q queue;
void ready_queue_init(rd_q *q);
extern void swtch(uint32_t *old_esp, uint32_t new_esp);
void schedule();
void yield();

#endif