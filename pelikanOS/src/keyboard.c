#include "keyboard.h"
#include "io.h"

#define RINGBUF_SIZE 256


static char scancode_to_ascii[128] = {
    0, 0, '1', '2', '3', '4', '5', '6',    // 0x00-0x07
    '7', '8', '9', '0', '-', '=', '\b', 0,     // 0x08-0x0F (0x0E=backspace, 0x0F=tab)
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', // 0x10-0x17
    'o', 'p', '[', ']', '\n', 0, 'a', 's',      // 0x18-0x1F (0x1C=enter, 0x1D=ctrl)
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  // 0x20-0x27
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',  // 0x28-0x2F (0x2A=lshift)
    'b', 'n', 'm', ',', '.', '/', 0, 0,       // 0x30-0x37
    0, ' ', 0,                                 // 0x38=alt, 0x39=space, 0x3A=caps
};
// 0 means uprintable char

typedef struct input_buffer{
    uint8_t data[RINGBUF_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t len;
}RB;

static RB kbd_rb;

static void ringbuf_init(RB *rb){
    rb -> head = 0;
    rb -> tail = 0;
    rb -> len = 0;
}


void keyboard_init(void){
    ringbuf_init(&kbd_rb);
}

static void ringbuf_push(RB *rb , uint8_t byte){
    if(rb->len == RINGBUF_SIZE){ //head is right at tail 
        return;
    }
    rb->data[rb->head] = byte;
    rb->head = (rb->head + 1) % RINGBUF_SIZE;
    rb->len++;
}

static uint8_t ringbuf_pop(RB *rb , uint8_t *out){
    if(rb->len == 0){
        return 0;
    }
    uint32_t flags = save_flags_and_cli(); 
    *out = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) % RINGBUF_SIZE;
    rb->len--;
    restore_flags(flags);
    return 1;
}

uint8_t kbd_getchar(uint8_t *out){ 
    return ringbuf_pop(&kbd_rb , out);
}

void keyboard_handler(void){
    uint8_t scancode = inb(0x60);
    if(scancode & 0x80) return;              // key release, ignore
    uint8_t key = scancode_to_ascii[scancode];
    if(key == 0) return;                      // unmapped key, drop
    ringbuf_push(&kbd_rb, key);               // deposit and leave
}