#include "mutex_lock.h"

void add_task_to_mutex_queue(task_t *task , mutex_t *lock){
    if(!isempty_wait_queue(lock)){
        task->mutex_wait_next = 0;
        lock->queue_tail->mutex_wait_next = task;
        lock->queue_tail = task;
    }else{
        lock->queue_head = task;
        lock->queue_tail = task;
    }  
}

task_t *remove_task_from_mutex_queue(mutex_t *lock){
    if(isempty_wait_queue(lock)){
        return 0;
    }
    task_t *task = lock->queue_head;
    if(task->mutex_wait_next == 0){
        lock->queue_head = 0;
        lock->queue_tail = 0;
    }else{
        lock->queue_head = task->mutex_wait_next;
    }
    task->mutex_wait_next = 0; //disconnect it from the FIFO list
    return task;
}

uint8_t isempty_wait_queue(mutex_t *lock){
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
    if(!isempty_wait_queue(lock)){
        task_t *task = remove_task_from_mutex_queue(lock);
        task->state = TASK_READY;
        add_task_to_ready_queue(&queue , task);
    }
    restore_flags(flags);
}

void mutex_init(mutex_t *lock){
    lock->locked = 0;
    lock->holder = 0;
    lock->queue_head = 0;
    lock->queue_tail = 0;
}