#include "heap.h"
#include "pmm.h"


typedef struct block{
    uint32_t size;
    uint32_t free;
    struct block *next;
    struct block *prev;
}block_t;

#define BLOCK_SIZE sizeof(block_t)

//head of the linked list
static block_t *head = 0; // 0==NULL

block_t *request_space(block_t *last, uint32_t size){
    uint32_t *raw = (uint32_t *) pmm_alloc_frame();
    if(raw == 0){
        char buf[32]= {'N' , 'U' , 'L' , 'L' , ' ' , ' ' , 'R' , 'E' , 'Q' , 'U' , 'E' , 'S' , 'T' , '\0'};
        print(buf);
        while(1) asm volatile("hlt");
    }
    block_t *block = (block_t *) raw;
    block->size = 4096 - sizeof(block_t);
    block->free = 0;
    block->next = 0;
    if(last){
        block->prev = last;
        last->next = block;
    }else{
        block->prev = 0;
    }

    return block;
}

void *kmalloc(uint32_t size){
    if(size == 0){
        return 0;
    }

    if(!head){
        block_t *block = request_space(0, size);
        if(!block){
            return 0;
        }
        head = block;
        return (void *) (block + 1); // return pointer to after the header
    }

    block_t *current = head;
    block_t *last = 0;
    
    while(current){
        if(current->free == 1){
            if(current->size == size){ //exact fit
                current->free = 0;
                return (void*) (current + 1); //returns memory after the header
            }else if(current->size >= size + BLOCK_SIZE + 1){ //enough space for header
                block_t *new_block = (block_t *)((uint8_t *)(current + 1) + size);
                new_block->size = current->size - size - BLOCK_SIZE;
                new_block->free = 1;
                new_block->next = current->next;
                new_block->prev = current;

                current->size = size;
                current->free = 0;
                current->next = new_block;

                return (void *)(current + 1); //returns memory after header
            }
        }
        last = current;
        current = current->next;
    }
    //no free space - call pmm_alloc_frame
    block_t *block = request_space(last, size);
    if(!block){
        return 0;
    }
    return (void *) (block + 1); // return pointer to after the header
    //add prints to each case to show what happened 
}



void kfree(void *ptr){
    if(!ptr){
        return;
    }

    block_t *block = (block_t *) ptr - 1; //return the pointer to the header
    block->free = 1;
    if(block->next && block->next->free == 1){
        block_t *next = block->next;
        block->size = block->size + BLOCK_SIZE + next->size;
        block->next = next->next; //the coalesced block shows to the nexts next block , could be null
        if(next->next){
            block->next->prev = block;
        }
    }
    if(block-> prev && block->prev->free == 1){
        block_t *prev = block->prev;
        prev -> size = prev -> size + BLOCK_SIZE + block->size; 
        prev -> next = block->next;   
        if(block->next){
            if(block->next->prev){
                block->next->prev = prev;
            }
        }
    }
    //print something to show it worked
}