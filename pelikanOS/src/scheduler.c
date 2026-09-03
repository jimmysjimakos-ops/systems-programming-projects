#include "scheduler.h"
#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "vga.h"

/*
typedef struct task{   //this should be encapsulated
    uint32_t ESP;
    uint32_t EIP;
    uint32_t *page_directory;
    uint8_t state;
    uint32_t *stack_base;
    uint32_t id;
    struct task *next;
    struct task *prev;
}task_t;
*/

static task_t *last_inserted;
task_t *current_task;

void insert_task(task_t *task){
    if(last_inserted){
        task->next = last_inserted->next; //link to the first task in the list
        task->prev = last_inserted;
        last_inserted->next->prev = task;
        last_inserted->next = task;
    }else{
        task->next = task; //link to itself i guess
        task->prev = task; //same
        current_task = task;
    }
    
    last_inserted = task;
}


task_t *create_task(void (execution_code)()){
    task_t *task = (task_t *) kmalloc(sizeof(task_t));
    task->stack_base = pmm_alloc_frame();
    task->ESP = task->stack_base + 4096; //stack grows down , so start from the end of the 4KB block and work up
    task->EIP = (uint32_t) execution_code;
    task->state = TASK_READY;
    task->page_directory = vmm_get_kernel_directory(); //page_dir in vmm is static and needs a getter func
    task->id = 1; //hardcode for now but gonna be incremental
    //when a new task is loaded for the first time , pop and ret read garbage --> must push fake data
    uint32_t *stack_top = (uint32_t *) task->ESP;
    *(--stack_top) = (uint32_t) execution_code; //esp points to one past the end of our frame , so we first decrement then store
    //also we put the func pointer first so when everything else gets popped , we ret into the func 
    *(--stack_top) = 0;  // fake EBP
    *(--stack_top) = 0;  // fake EDI
    *(--stack_top) = 0;  // fake ESI
    *(--stack_top) = 0;  // fake EBX  
    task->ESP = (uint32_t) stack_top;
    insert_task(task);
    return task; //optional , just if i need it in the future 
}


void schedule(){
    if(!current_task){
        char buf[32] = "NO TASK \n";
        print(buf);
        return;
    }
    task_t *next = current_task->next;
    context_switch(next);
}