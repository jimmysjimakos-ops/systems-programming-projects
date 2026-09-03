#include <stdint.h>
#define TASK_READY   0
#define TASK_RUNNING 1
#define TASK_DEAD    2

typedef struct task{   //this should be encapsulated
    uint32_t ESP;
    uint32_t EIP;
    uint32_t page_directory;
    uint8_t state;
    uint32_t stack_base;
    uint32_t id;
    struct task *next;
    struct task *prev;
}task_t;

task_t *create_task(void (*func)());
void insert_task(task_t *task);
void remove_task(uint32_t *task);
extern void context_switch(task_t *task); 