section .text
extern current_task;
global context_switch

context_switch:     ;esp is my first field so no offset needed
    push EBX;
    push ESI;
    push EDI;
    push EBP;
    mov  eax , [current_task] ;
    mov  [eax] , esp;
    mov  eax , [esp+20]; ;NEXT TASK
    mov  [current_task] , eax; ;MAKE CURRENT_TASK HOLD THE NEXT TASK FOR FUTURE CONTEXT SWITCHES
    mov esp , [eax]; ;LOAD THE NEW TASKS STACK
    pop EBP;
    pop EDI;
    pop ESI;
    pop EBX; 
    sti  ;enable interrupts again so task switching is possible, they disable on each ISR call
    ret;