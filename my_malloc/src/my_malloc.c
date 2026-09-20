#include <unistd.h>     //gives access to sbrk
#include "../include/my_malloc.h"

//this is the structure of every memory block?

typedef struct block {
    size_t size; //how many bytes
    int free;   //1=free , 0=in use
    struct block *next;
} block_t;

#define BLOCK_SIZE sizeof(block_t)

//head of the linked list(yes its a linked list)
static block_t *head=NULL;

void *my_malloc(size_t size){
    if(size == 0){
        return NULL;
    }

    void *raw = sbrk(BLOCK_SIZE + size);
    if (raw == (void *) -1){
        return NULL; //sbrk failed
    }

    block_t *block = (block_t *) raw; //make a  block prt that poits to the same point as raw


    block -> size = size; //set the headers data
    block -> free = 0;
    block -> next = NULL;
    
    if(!head){
        head=block; //make it the first bookmark if no others exist
    }else{          //else append it to the last one
        block_t *current = head;
        while(current->next){
            current = current->next;
        }
        current->next=block;
    }

    return (void *)(block + 1);//sbrk(raw) points to the start of the 124 bytes , we make a block_t pointer that points to the same place and we increment +1(24bytes) to get the actual start of the free space(100bytes) and then we return that afetr casting it to void 

}

void my_free(void*ptr){
    if(!ptr){
        return;
    }
    block_t *block = (block_t *) ptr -1;
    block ->free = 1; 
}