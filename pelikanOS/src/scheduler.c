#include "scheduler.h"
#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "vga.h"

static int num_of_tasks;
static task_t *last_inserted;
task_t *current_task;
rd_q queue = {0,0};



void ready_queue_init(rd_q *q){  //may stand unused
    q->queue_head = 0;
    q->queue_tail = 0;
}


void insert_task(task_t *task){
    if(last_inserted){
        task->next = last_inserted->next; //link to the first task in the list
        task->prev = last_inserted;
        last_inserted->next->prev = task;
        last_inserted->next = task;
    }else{
        task->next = task; //link to itself i guess
        task->prev = task; //same
    }
    
    last_inserted = task;
}


task_t *create_task(void (execution_code)() , int id){
    task_t *task = (task_t *) kmalloc(sizeof(task_t));
    uint32_t base = pmm_alloc_contiguous(2);
    vmm_unmap_page(base);
    // guard = base + 4096 guard is 1 frame 
    task->stack_base = base + 4096;
    task->ESP = task->stack_base + 4096;
    task->EIP = (uint32_t) execution_code;
    task->page_directory = vmm_get_kernel_directory(); //page_dir in vmm is static and needs a getter func
    task->id = id; 
    //when a new task is loaded for the first time , pop and ret read garbage --> must push fake data
    uint32_t *stack_top = (uint32_t *) task->ESP;
    *(--stack_top) = (uint32_t) execution_code; //esp points to one past the end of our frame , so we first decrement then store
    //also we put the func pointer first so when everything else gets popped , we ret into the func 
    *(--stack_top) = 0x202;                        // EFLAGS
    *(--stack_top) = 0;  // EBX
    *(--stack_top) = 0;  // ESI
    *(--stack_top) = 0;  // EDI
    *(--stack_top) = 0;  // EBP 
    task->ESP = (uint32_t) stack_top;
    /*
    if(current_task == 0){
        task->state = TASK_RUNNING; //idle task , task 0
        current_task = task;
    }else{  
        task->state = TASK_READY; //not idle task , other
        add_task_to_ready_queue(&queue , task); 
    }
    */
    task->state = TASK_READY;
    add_task_to_ready_queue(&queue, task);
    insert_task(task);
    num_of_tasks++;
    //defence field init
    task->next = 0;
    task->prev = 0;
    task->ready_next = 0;
    task->mutex_wait_next = 0;
    return task; //optional , just if i need it in the future 
}

int get_num_of_tasks(){
    return num_of_tasks;
}

task_t *get_current_task(){
    return current_task;
} 

uint8_t isempty_ready_queue(rd_q *q){
    return q->queue_head == 0;
}

void add_task_to_ready_queue(rd_q *q , task_t *task){
    if(!isempty_ready_queue(q)){
        task->ready_next = 0;
        q->queue_tail->ready_next = task;
        q->queue_tail = task;
    }else{
        q->queue_head=task;
        q->queue_tail=task;
    }
}


task_t *remove_task_from_ready_queue(rd_q *q){
    if(isempty_ready_queue(q)){
        return 0;
    }
    task_t *task = q->queue_head;
    if(task->ready_next== 0){
        q->queue_head = 0;
        q->queue_tail = 0;
    }else{
        q->queue_head = task->ready_next;
    }
    task->ready_next = 0; //disconnect it from the FIFO list
    return task;
}

/*
void schedule(){
    if(!current_task) return;
    if(isempty_ready_queue(&queue)) return;  // idle is never dequed
    
    if(current_task->state == TASK_RUNNING){
        current_task->state = TASK_READY;
        add_task_to_ready_queue(&queue , current_task);
    }
    
    task_t *task = remove_task_from_ready_queue(&queue);
    task->state = TASK_RUNNING;
    context_switch(task); //current task is updated in asm
}
*/


uint32_t scheduler_context;
void schedule(){
    while(1){
        task_t *next = remove_task_from_ready_queue(&queue);
        if(!next){
            continue;
        }

        next->state = TASK_RUNNING;
        current_task = next;
        swtch(&scheduler_context, next->ESP); //rets here when it yields back
        //decide what to do with the current task
        if(current_task->state == TASK_RUNNING){
            current_task->state = TASK_READY;
            add_task_to_ready_queue(&queue, current_task);
        }
    }
}


void yield(){
    swtch(&current_task->ESP, scheduler_context);
}