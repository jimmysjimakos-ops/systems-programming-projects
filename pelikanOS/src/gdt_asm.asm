section .text
extern gdt_ptr 
global gdt_load 

gdt_load:
    lgdt [gdt_ptr]

    jmp 0x08:flush_cs
    flush_cs:
        mov ax , 0x10
        mov ds , ax
        mov es , ax
        mov fs , ax
        mov gs , ax
        mov ss , ax
        ret