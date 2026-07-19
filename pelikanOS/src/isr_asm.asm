section .text

%macro ISR_NO_ERR 1 ;macro is used to shorten code , in each stub i either push an error code 0 and interrupt num, 
    isr%1:          ;or just the interrupt num cuz some interrupts push error code automatically
        push dword 0 
        push dword %1
        jmp isr_common
%endmacro

%macro ISR_ERR 1
    isr%1:
        push dword %1
        jmp isr_common
%endmacro

ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_NO_ERR 9
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 31
ISR_NO_ERR 32
ISR_NO_ERR 33
ISR_NO_ERR 34
ISR_NO_ERR 35
ISR_NO_ERR 36
ISR_NO_ERR 37
ISR_NO_ERR 38
ISR_NO_ERR 39
ISR_NO_ERR 40
ISR_NO_ERR 41
ISR_NO_ERR 42
ISR_NO_ERR 43
ISR_NO_ERR 44
ISR_NO_ERR 45
ISR_NO_ERR 46
ISR_NO_ERR 47
ISR_ERR 8
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_ERR 17
ISR_ERR 21
ISR_ERR 29
ISR_ERR 30


extern isr_handler ; Handler in c code to handle faults

isr_common:  ;every stub maps to this stub which does important things
    pusha    ;pushes all 8 general-registers to the stack in order to save their values , else they would be overwritten in my c handler
    push esp ;make esp point to the current stack top , just after all the registers,before the ir num and err_code
    call isr_handler ;calls the c handler
    add esp, 4 ; remove pushed esp argument ( before i had it wrong , thinking i skipped the ir_num and err code here)
    popa     ;Restores the values of the pushed all 8 general registers
    add esp, 8  ;adds 8 bytes to the stack pointer top , cuz we have 2 error codes sitting there from the previous stubs isr% (the 2 dd words) basically skips 4 bytes 2 times
    iret     ;Special return for handlers that pops from EIP,CS,EFLAGS cuz normal rets only pop from EIP , then it continues the interrupted code with  only the saved stuff on the stack like normal
;btw esp is just a register , or a pointer that points to the top of the stack (EIP when in an interrupt , cuz thats what gets pushed to the stack on each interrupt)

section .data
global isr_table ;the stubs all piled up for my C idt file  

isr_table:
    dd isr0
    dd isr1
    dd isr2
    dd isr3
    dd isr4
    dd isr5
    dd isr6
    dd isr7
    dd isr8
    dd isr9
    dd isr10
    dd isr11
    dd isr12
    dd isr13
    dd isr14
    dd isr15
    dd isr16
    dd isr17
    dd isr18
    dd isr19
    dd isr20
    dd isr21
    dd isr22
    dd isr23
    dd isr24
    dd isr25
    dd isr26
    dd isr27
    dd isr28
    dd isr29
    dd isr30
    dd isr31
    dd isr32
    dd isr33
    dd isr34
    dd isr35
    dd isr36
    dd isr37
    dd isr38
    dd isr39
    dd isr40
    dd isr41
    dd isr42
    dd isr43
    dd isr44
    dd isr45
    dd isr46
    dd isr47


    ;mental mode below 

;[esp+0]   return address (from call)
;[esp+4]   edi  ┐
;[esp+8]   esi  │
;[esp+12]  ebp  │
;[esp+16]  esp  │ ← pusha (32 bytes)
;[esp+20]  ebx  │
;[esp+24]  edx  │
;[esp+28]  ecx  │
;[esp+32]  eax  ┘
;[esp+36]  interrupt number
;[esp+40]  error code
;[esp+44]  EIP  ┐
;[esp+48]  CS   │ ← pushed by CPU
;[esp+52]  EFLAGS ┘ 