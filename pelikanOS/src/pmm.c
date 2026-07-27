#include "pmm.h"
#include "string.h"
#define MAX_FRAMES 32768

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

uint8_t bitmap[4096]; //memset it


void pmm_init(struct multiboot_info *mboot){
    uint32_t length = mboot->mmap_length; 
    uint32_t start_addr = mboot->mmap_addr;
    struct multiboot_mmap_entry *entry;
    uint32_t start_frame;
    uint32_t end_frame;
    memset(bitmap, 0xFF, sizeof(bitmap));

    uint32_t current = mboot->mmap_addr;

    while(current < start_addr + length){
        entry = (struct multiboot_mmap_entry *)current;
        if(entry->type == 1){
            start_frame = entry->addr / 4096;
            end_frame = (entry->addr + entry->len) / 4096; 
            for(uint32_t i = start_frame; i < end_frame; i++){
                bitmap[i / 8] &= ~(1 << (i % 8));
            }
        }
        current += entry->size + 4; //multiboot quirk , some stupid shit
    }

    // Protect first 1MB
    for(int i = 0; i < 256; i++){  // 256 frames = 1MB
        bitmap[i / 8] |= (1 << (i % 8));
    }

    uint32_t kernel_start = (uint32_t)&_kernel_start; 
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    start_frame = kernel_start / 4096;
    end_frame = kernel_end / 4096;

    for(uint32_t frame = start_frame; frame < end_frame; frame++){
        bitmap[frame / 8] |= (1 << (frame % 8));
    }
}

uint32_t pmm_alloc_frame(){
    for(int frame=0; frame< MAX_FRAMES; frame++){
        if(!(bitmap[frame / 8] & (1 << (frame % 8)))){
            bitmap[frame / 8] |= (1 << (frame % 8)); //set bit to used
            return frame * 4096; //retruns physical address
        }
    }
    return 0xFFFFFFFF;
}

void pmm_free_frame(uint32_t frame_addr){
    uint32_t frame_num;
    frame_num = frame_addr / 4096;
    bitmap[frame_num / 8] &= ~(1 << (frame_num % 8));
}