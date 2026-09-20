#include "mutex_lock.h"
volatile int logpos = 0;
void logevent(char op, int cur, int holder, int locked){
    int base = 400 + (logpos % 16) * 4;   // row 5, 16 slots of 4 cells
    ((volatile uint16_t*)0xB8000)[base]   = (0x0F<<8)|op;
    ((volatile uint16_t*)0xB8000)[base+1] = (0x0F<<8)|('0'+cur);
    ((volatile uint16_t*)0xB8000)[base+2] = (0x0F<<8)|('0'+holder);
    ((volatile uint16_t*)0xB8000)[base+3] = (0x0F<<8)|('0'+locked);
    logpos++;
}


void add_task_to_mutex_queue(task_t *task , mutex_t *lock){
    if(!isempty(lock)){
        task->mutex_next = 0;
        lock->queue_tail->mutex_next = task;
        lock->queue_tail = task;
    }else{
        lock->queue_head = task;
        lock->queue_tail = task;
    }  
}

task_t *remove_task_from_mutex_queue(mutex_t *lock){
    if(isempty(lock)){
        return 0;
    }
    task_t *task = lock->queue_head;
    if(task->mutex_next == 0){
        lock->queue_head = 0;
        lock->queue_tail = 0;
    }else{
        lock->queue_head = task->mutex_next;
    }
    task->mutex_next = 0; //disconnect it from the FIFO list
    return task;
}

uint8_t isempty(mutex_t *lock){
    return lock->queue_head == 0;
}

void mutex_lock(mutex_t *lock){
    uint32_t flags = save_flags_and_cli();
    while(lock->locked){
        task_t *me = get_current_task();
        me->state = TASK_BLOCKED;
        add_task_to_mutex_queue(me, lock);
        schedule();                 // interrupts still OFF; switch away
        // resumed here after being woken; loop re-checks locked
    }
    lock->locked = 1;
    lock->holder = get_current_task();
    restore_flags(flags);
}

void mutex_unlock(mutex_t *lock){
    uint32_t flags = save_flags_and_cli();
    lock->locked = 0;
    lock->holder = 0;
    if(!isempty(lock)){
        task_t *task = remove_task_from_mutex_queue(lock);
        task->state = TASK_READY;
    }
    restore_flags(flags);
}

void mutex_init(mutex_t *lock){
    lock->locked = 0;
    lock->holder = 0;
    lock->queue_head = 0;
    lock->queue_tail = 0;
}