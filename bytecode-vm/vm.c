#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
    OP_PUSH = 0x01, //push byte on to stack
    OP_POP = 0x02, //discard byte from stack
    OP_ADD = 0x03, // pop two bytes and add them
    OP_SUB = 0x04, //pop 2 bytes and sub them
    OP_MUL = 0x05, //pop 2 and multiply
    OP_DIV = 0x06, //pop 2 and div
    OP_PRINT = 0x07, //pop and print top
    OP_EQ = 0x08,
    OP_LT = 0x09,
    OP_GT = 0x0A,
    OP_JMP = 0x0B,
    OP_JMP_IF_FALSE = 0x0C,
    OP_LOAD = 0x0D,
    OP_STORE = 0x0E,
    OP_CALL = 0x0F,
    OP_RET = 0x1F,
    OP_HALT = 0xFF, //stop vm
}OpCode;

typedef struct{
    uint8_t *code;      //bytecode array
    size_t ip;        //instruction pointer for next instruction
    int stack[256];  //stack for 
    int sp;         //stack pointer
    int globals[256]; 
    int cstack[256];
    int csp;
    bool running;
}VM;

void vm_init(VM *vm, uint8_t *code){
    if(code == NULL){
        fprintf(stderr , "INIT: BYTECODE ARRAY EMPTY\n");
        return;
    }

    vm->code = code;
    vm->ip = 0;
    vm->sp = 0;
    vm->csp = 0;
    memset(vm->globals, 0, sizeof(vm->globals));
    memset(vm->stack, 0, sizeof(vm->stack));
    memset(vm->cstack, 0, sizeof(vm->cstack));
    vm->running = true;
    printf("VM INITIALIZED SUCCESSFULLY\n");
}

void vm_push(VM *vm, int num){
    if(vm->sp == 256){
        fprintf(stderr, "CANNOT PUSH --- STACK OVERFLOW\n");
        exit(1);
    }
    vm->stack[vm->sp] = num;
    vm->sp++;
}

int vm_pop(VM *vm){
    if(vm->sp <= 0){
        fprintf(stderr, "CANNOT POP --- STACK OVERFLOW\n");
        exit(1);
    }

    vm->sp--;
    int popped = vm->stack[vm->sp]; 
    return popped;
}

void vm_push_cs(VM *vm, int ip){
    if(vm->csp == 256){
        fprintf(stderr, "CANNOT CPUSH --- STACK OVERFLOW\n");
        exit(1);
    }
    vm->cstack[vm->csp] = ip;
    vm->csp++;
}

int vm_pop_cs(VM *vm){
    if(vm->csp <= 0){
        fprintf(stderr, "CANNOT CPOP --- STACK OVERFLOW\n");
        exit(1);
    }

    vm->csp--;
    int popped = vm->cstack[vm->csp]; 
    return popped;
}

void vm_run(VM *vm) {
    while (vm->running) {
        uint8_t op = vm->code[vm->ip++];
        printf("now running op : %u \n" , op);    // fetch

        switch (op) {                        // decode + execute
            case OP_PUSH: {
                // operand is the next byte in the code stream
                int value = (int)vm->code[vm->ip++];    //fetches then increments
                vm_push(vm, value);
                break;
            }
            case OP_POP:
                vm_pop(vm);
                break;
            case OP_ADD: {
                int b = vm_pop(vm);
                int a = vm_pop(vm);
                vm_push(vm, a + b);
                break;
            }
            case OP_SUB: {
                int b = vm_pop(vm);
                int a = vm_pop(vm);
                vm_push(vm, a - b);
                break;
            }
            case OP_MUL: {
                int b = vm_pop(vm);
                int a = vm_pop(vm);
                vm_push(vm, a * b);
                break;
            }
            case OP_DIV: {
                int b = vm_pop(vm);
                int a = vm_pop(vm);
                if (b == 0) {
                    fprintf(stderr, "division by zero\n");
                    exit(1);
                }
                vm_push(vm, a / b);
                break;
            }
            case OP_EQ: {
                int res;
                int a = vm_pop(vm);
                int b = vm_pop(vm);
                if(a == b){
                    res = 1;
                }else{
                    res = 0;
                }
                vm_push(vm, res);
                break;
            }
            case OP_GT: {
                int res;
                int a = vm_pop(vm);
                int b = vm_pop(vm);
                if(a < b){
                    res = 1;
                }else{
                    res = 0;
                }
                vm_push(vm, res);
                break;
            }
            case OP_LT: {
                int res;
                int a = vm_pop(vm);
                int b = vm_pop(vm);
                if(a > b){
                    res = 1;
                }else{
                    res = 0;
                }
                vm_push(vm, res);
                break;
            }
            case OP_JMP: {
                vm->ip = vm->code[vm->ip];
                break;
            }
            case OP_JMP_IF_FALSE: {
                int cond = vm_pop(vm);
                if(cond){
                    vm->ip++;
                }else{
                    vm->ip = vm->code[vm->ip];
                }
                break;
            }
            case OP_STORE: {
                int val = vm_pop(vm);
                vm->globals[vm->code[vm->ip++]] = val;
                break;
            }
            case OP_LOAD: {
                int val = vm->globals[vm->code[vm->ip++]];
                vm_push(vm, val);
                break;
            }
            case OP_CALL: {
                uint8_t dest = vm->code[vm->ip++];  
                vm_push_cs(vm, vm->ip);             
                vm->ip = dest;                       
                break;
            }
            case OP_RET: {
                int ip = vm_pop_cs(vm);
                vm->ip = ip;
                break;
            }
            case OP_PRINT: {
                int value = vm_pop(vm);
                printf("%d\n", value);
                break;
            }
            case OP_HALT:
                vm->running = false;
                break;
            default:
                fprintf(stderr, "unknown opcode: 0x%02x\n", op);
                exit(1);
        }
    }
}





