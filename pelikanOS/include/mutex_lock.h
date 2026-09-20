#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include "scheduler.h"
#include "io.h"


typedef struct mutex{
    int locked;
    task_t *holder;
    task_t *queue_head;
    task_t *queue_tail;
}mutex_t;

uint8_t isempty(mutex_t *lock);
void add_task_to_mutex_queue(task_t *task, mutex_t *lock);
task_t *remove_task_from_mutex_queue(mutex_t *lock);
void mutex_lock(mutex_t *lock);
void mutex_unlock(mutex_t *lock); 
void mutex_init(mutex_t *lock);

#endif