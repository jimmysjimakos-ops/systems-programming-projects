ENTRY(_start)

SECTIONS {
    . = 1M;

    _kernel_start = .;

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }
    
    .stack (NOLOAD) : ALIGN(16) {
        _stack_bottom = .;
        *(.stack)
        _stack_top = .;
        . += 4096;          /* guard gap: nothing lands at stack_top */
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }

    _kernel_end = .;

}