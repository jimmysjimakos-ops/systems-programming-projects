; define constants
MODULEALIGN equ 1 << 0
MEMINFO equ 1 << 1
FLAGS equ MODULEALIGN | MEMINFO ;grub needs just an int nothing more , with this we do 0001 | 0010 = 0011 
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS) ;Guard calc required by multiboot

;Multiboot header 

section .multiboot
align 4 ;ensures the output bytes will be a multiple of 4(pads if needed)
    dd MAGIC  ;dd writes 4 bytes 
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384 
stack_top:

section .text
global _start
extern kmain

_start:
    mov esp , stack_top;
    push ebx
    push eax

    call kmain

    cli ;disables interrupts
.hang:
    hlt
    jmp .hang