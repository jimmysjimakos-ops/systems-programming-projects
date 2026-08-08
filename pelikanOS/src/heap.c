#include "heap.h"
#include "pmm.h"
#include "itoa.h"
#include "vga.h"

typedef struct block{
    uint32_t size;
    uint32_t free;
    struct block *next;
    struct block *prev;
}block_t;

#define BLOCK_SIZE sizeof(block_t)

static block_t *head = 0;

block_t *request_space(block_t *last, uint32_t size){
    uint32_t *raw = (uint32_t *) pmm_alloc_frame();
    if(raw == 0){
        print("HEAP: request_space FAILED - NULL from PMM\n");
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
    char buf[32];
    print("HEAP: new frame at ");
    itoa((uint32_t)raw, buf);
    print(buf);
    print("\n");
    return block;
}

void *kmalloc(uint32_t size){
    if(size == 0){
        print("HEAP: kmalloc(0) ignored\n");
        return 0;
    }

    if(!head){
        block_t *block = request_space(0, size);
        if(!block){
            print("HEAP: first alloc FAILED\n");
            return 0;
        }
        head = block;
        char buf[32];
        print("HEAP: first alloc, size ");
        itoa(size, buf);
        print(buf);
        print("\n");
        return (void *) (block + 1);
    }

    block_t *current = head;
    block_t *last = 0;

    while(current){
        if(current->free == 1){
            if(current->size == size){
                current->free = 0;
                char buf[32];
                print("HEAP: exact fit, size ");
                itoa(size, buf);
                print(buf);
                print("\n");
                return (void*) (current + 1);
            }else if(current->size >= size + BLOCK_SIZE + 1){
                block_t *new_block = (block_t *)((uint8_t *)(current + 1) + size);
                new_block->size = current->size - size - BLOCK_SIZE;
                new_block->free = 1;
                new_block->next = current->next;
                new_block->prev = current;

                current->size = size;
                current->free = 0;
                current->next = new_block;

                char buf[32];
                print("HEAP: split, size ");
                itoa(size, buf);
                print(buf);
                print("\n");
                return (void *)(current + 1);
            }
        }
        last = current;
        current = current->next;
    }

    block_t *block = request_space(last, size);
    if(!block){
        print("HEAP: alloc FAILED, no space\n");
        return 0;
    }
    char buf[32];
    print("HEAP: new frame alloc, size ");
    itoa(size, buf);
    print(buf);
    print("\n");
    return (void *) (block + 1);
}

void kfree(void *ptr){
    if(!ptr){
        print("HEAP: kfree(NULL) ignored\n");
        return;
    }

    block_t *block = (block_t *) ptr - 1;
    block->free = 1;
    char buf[32];
    print("HEAP: freed block, size ");
    itoa(block->size, buf);
    print(buf);

    if(block->next && block->next->free == 1){
        block_t *next = block->next;
        block->size = block->size + BLOCK_SIZE + next->size;
        block->next = next->next;
        if(next->next){
            block->next->prev = block;
        }
        print(" +coalesced next");
    }
    if(block->prev && block->prev->free == 1){
        block_t *prev = block->prev;
        prev->size = prev->size + BLOCK_SIZE + block->size;
        prev->next = block->next;
        if(block->next){
            if(block->next->prev){
                block->next->prev = prev;
            }
        }
        print(" +coalesced prev");
    }
    print("\n");
}