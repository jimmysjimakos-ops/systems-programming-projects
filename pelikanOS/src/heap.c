#include "heap.h"
#include "pmm.h"
#include "itoa.h"
#include "vga.h"

typedef struct block{
    uint32_t size;
    uint32_t free;
    uint32_t magic;
    struct block *next;
    struct block *prev;
}block_t;

#define BLOCK_SIZE sizeof(block_t)
#define HEAP_MAGIC_ALLOC 0xDEADBEEF
#define HEAP_MAGIC_FREE 0xFEEEFEEE

static block_t *head = 0;

block_t *request_space(block_t *last, uint32_t size){
    uint32_t *raw = (uint32_t *) pmm_alloc_frame();
    if(raw == 0){
        print("HEAP: request_space FAILED - NULL from PMM\n");
        while(1) asm volatile("hlt");
    }
    block_t *block = (block_t *) raw;
    block->size = 4096 - sizeof(block_t);
    block->free = 1;
    block->magic = HEAP_MAGIC_FREE;
    block->next = 0;
    if(last){
        block->prev = last;
        last->next = block;
    }else{
        block->prev = 0;
    }
    //char buf[32];
    //print("HEAP: new frame at ");
    //itoa((uint32_t)raw, buf);
    //print(buf);
    //print("\n");
    return block;
}

void *kmalloc(uint32_t size){
    if(size == 0){
        print("HEAP: kmalloc(0) ignored\n");
        return 0;
    }
    if(size > 4096 - BLOCK_SIZE){ 
        print("HEAP: too big\n"); 
        return 0;
    }

    if(!head){
        head = request_space(0, size);
    }

    block_t *current = head;
    block_t *last = 0;
    while(1){
        while(current){
            if(current->free == 1){
                if(current->size == size){
                    current->free = 0;
                    //char buf[32];
                    //print("HEAP: exact fit, size ");
                    //itoa(size, buf);
                    //print(buf);
                    //print("\n");
                    current->magic = HEAP_MAGIC_ALLOC;
                    return (void*) (current + 1);
                }else if(current->size >= size + BLOCK_SIZE + 1){
                    block_t *new_block = (block_t *)((uint8_t *)(current + 1) + size);
                    new_block->size = current->size - size - BLOCK_SIZE;
                    new_block->free = 1;
                    new_block->next = current->next;
                    new_block->prev = current;
                    new_block->magic = HEAP_MAGIC_FREE;

                    current->size = size;
                    current->free = 0;
                    current->next = new_block;

                    //char buf[32];
                    //print("HEAP: split, size ");
                    //itoa(size, buf);
                    //print(buf);
                    //print("\n");
                    current->magic = HEAP_MAGIC_ALLOC;
                    return (void *)(current + 1);
                }else if(current->size > size){
                    current->free = 0;
                    current->magic = HEAP_MAGIC_ALLOC;
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
        current = block; //start the process from the newly allocated frame
        
        //char buf[32];
        //print("HEAP: new frame alloc, size ");
        //itoa(size, buf);
        //print(buf);
        //print("\n");
    }
}

void kfree(void *ptr){ //this needs to also check if the two blocks about to be coalesced ARE IN FACT ADJACENT from physical 
    if(!ptr){
        print("HEAP: kfree(NULL) ignored\n");
        return;
    }

    block_t *block = (block_t *) ptr - 1;

    //chech if theres either stamp(is it one of my blocks)
    if(block->magic != HEAP_MAGIC_ALLOC && block->magic != HEAP_MAGIC_FREE){
        print("HEAP: bad free - not my block\n"); asm volatile("hlt");
    }
    //check if its the free stamp(already freed)
    if(block->magic == HEAP_MAGIC_FREE){
        print("HEAP: double free\n"); asm volatile("hlt");
    }
    block->free = 1;
    block->magic = HEAP_MAGIC_FREE;   // now it carries the freed-password
    //the above is to avoid leftover magic fields and to give accurate diagnostics
    
    char buf[32];
    print("HEAP: freed block, size ");
    itoa(block->size, buf);
    print(buf);

    if(block->next && block->next->free == 1 && (uint8_t *) block + BLOCK_SIZE + block->size == (uint8_t *) block->next){
        block_t *next = block->next;
        block->size = block->size + BLOCK_SIZE + next->size;
        block->next = next->next;
        if(next->next){
            block->next->prev = block;
        }
        print(" +coalesced next");
    }
    if(block->prev && block->prev->free == 1 && (uint8_t *) block->prev + BLOCK_SIZE + block->prev->size == (uint8_t *) block){
        block_t *prev = block->prev;
        prev->size = prev->size + BLOCK_SIZE + block->size;
        prev->next = block->next;
        if(block->next){
            block->next->prev = prev;
            
        }
        print(" +coalesced prev");
    }
    print("\n");
}

void heap_dump(){
    if(!head){
        print("NO LIST??");
        return;
    }
    block_t *current = head;
    uint32_t current_block;
    char buf[20];
    char buf2[20];
    char buf3[20];
    while (current){
        current_block = (uint32_t) current;
        itoa(current_block , buf);
        itoa(current->size , buf2);
        itoa(current->free , buf3);
        print(buf);
        print("  ");
        print(buf2);
        print("  ");
        print(buf3);
        print("  ");
        print("\n");
        current = current->next;
    }
}