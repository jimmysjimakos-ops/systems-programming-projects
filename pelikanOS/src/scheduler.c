#include "scheduler.h"
#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "vga.h"
#include "string.h"
#include "io.h"

static int num_of_tasks;
static task_t *last_inserted;
task_t *current_task;
task_t *reaper_task;
rd_q queue = {0,0};
dd_q dead_q = {0,0};

void insert_task(task_t *task){
    uint32_t flags = save_flags_and_cli();
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
    restore_flags(flags);
}

void remove_task_from_circular_list(task_t *task){
    uint32_t flags = save_flags_and_cli();
    if(task->next == task){          // only node in the list
        last_inserted = 0;
    }else{
        task->prev->next = task->next;
        task->next->prev = task->prev;
        if(last_inserted == task){
            last_inserted = task->next; //repoint to surviving node
        }
    }
    task->next = 0;
    task->prev = 0;
    restore_flags(flags);
}


task_t *create_task(void (execution_code)() , int id , int type){
    task_t *task = (task_t *) kmalloc(sizeof(task_t));
    memset(task, 0, sizeof(task_t));   // every field starts clean
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
    if(type == 1){
        task->state = TASK_BLOCKED;
        reaper_task = task;
    }else if(type == 0){
        task->state = TASK_READY;
        add_task_to_ready_queue(&queue, task);
    }
    insert_task(task);
    num_of_tasks++;
    return task; //optional , just if i need it in the future 
}

int get_num_of_tasks(){
    return num_of_tasks;
}

task_t *get_current_task(){
    return current_task;
} 

static const char *state_name(uint8_t s){
    switch(s){
        case TASK_BLOCKED: return "BLOCKED";
        case TASK_READY:   return "READY";
        case TASK_RUNNING: return "RUNNING";
        case TASK_DEAD:    return "DEAD";
        default:           return "?";
    }
}

void scheduler_list_tasks(void){
    if(!last_inserted){
        print("no tasks\n");
        return;
    }
    task_t *spawnpoint = current_task;
    char buf[32];
    do{
        print("id: ");
        itoa(spawnpoint->id, buf);
        print(buf);
        print("  state: ");
        print(state_name(spawnpoint->state));   // see below
        print("\n");
        spawnpoint = spawnpoint->next;
    }while(spawnpoint != current_task);

}

uint8_t isempty_ready_queue(rd_q *q){
    return q->queue_head == 0;
}

void add_task_to_ready_queue(rd_q *q , task_t *task){
    uint32_t flags = save_flags_and_cli();
    task->ready_next = 0;  
    if(!isempty_ready_queue(q)){
        q->queue_tail->ready_next = task;
        q->queue_tail = task;
    }else{
        q->queue_head=task;
        q->queue_tail=task;
    }
    restore_flags(flags);
}


task_t *remove_task_from_ready_queue(rd_q *q){
    uint32_t flags = save_flags_and_cli();
    if(isempty_ready_queue(q)){
        restore_flags(flags);
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
    restore_flags(flags);
    return task;
}

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
        }else if(current_task->state == TASK_DEAD){
            add_task_to_dead_queue(&dead_q , current_task);
            if(reaper_task->state == TASK_BLOCKED){
                reaper_task->state = TASK_READY;
                add_task_to_ready_queue(&queue, reaper_task);
            }
            
        }
    }
}


void yield(){
    swtch(&current_task->ESP, scheduler_context);
}

void thread_exit(){
    current_task->state = TASK_DEAD;
    yield();
}

uint8_t isempty_dead_queue(dd_q *q){
    return q->dead_head == 0;
}

void add_task_to_dead_queue(dd_q *q , task_t *task){
    uint32_t flags = save_flags_and_cli();
    task->dead_next = 0;  
    if(!isempty_dead_queue(q)){
        q->dead_tail->dead_next = task;
        q->dead_tail = task;
    }else{
        q->dead_head=task;
        q->dead_tail=task;
    }
    restore_flags(flags);
}


task_t *remove_task_from_dead_queue(dd_q *q){
    uint32_t flags = save_flags_and_cli();
    if(isempty_dead_queue(q)){
        restore_flags(flags);
        return 0;
    }
    task_t *task = q->dead_head;
    if(task->dead_next== 0){
        q->dead_head = 0;
        q->dead_tail = 0;
    }else{
        q->dead_head = task->dead_next;
    }
    task->dead_next = 0; //disconnect it from the FIFO list
    restore_flags(flags);
    return task;
}

static void reaper_thread(void){
    asm volatile("sti");
    while(1){
        uint32_t flags = save_flags_and_cli();
        task_t *corpse = remove_task_from_dead_queue(&dead_q);
        if(!corpse){
            reaper_task->state = TASK_BLOCKED;
            restore_flags(flags);
            yield();
            continue;
        }
        remove_task_from_circular_list(corpse); //unlink
        uint32_t guard = corpse->stack_base - 4096;
        vmm_map_page(guard, guard);          // restore identity mapping on the guard frame
        pmm_free_frame(guard);
        pmm_free_frame(corpse->stack_base);
        kfree(corpse);
    }
}


void spawn_reaper(void){   //reaper API
    create_task(reaper_thread, 99, 1);
}