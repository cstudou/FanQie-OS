#include "init.h"
void init_all()
{
    Puts("init all!\n");
    idt_init();
    timer_init();
    memory_init();
    thread_init();
    console_init();
    keyboard_init();
    tss_init();
    On_interrupt();
    syscall_init();
    
    ide_init();
    filesystem_init();
}