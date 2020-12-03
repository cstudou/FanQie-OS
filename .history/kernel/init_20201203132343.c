#include "init.h"
void init_all()
{
    Puts("init all!\n");
    idt_init();
    timer_init();
    memory_init();
}