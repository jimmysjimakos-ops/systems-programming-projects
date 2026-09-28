section .text
global swtch;

swtch:
    push EBX;
    push ESI;
    push EDI;
    push EBP; ;push to current stack

    mov  eax , [esp + 20]; ;grab old_esp pointer from stack
    mov  [eax] , esp;  ;make the current stack pointer (from c side) point at the current stack  , kinda like marking 

    mov eax , [esp + 24]; ;grab new_esp
    mov esp , eax; ;load it into the esp 

    pop EBP;
    pop EDI;
    pop ESI;
    pop EBX; ;pop all the saved registers from the new stack

    ret;