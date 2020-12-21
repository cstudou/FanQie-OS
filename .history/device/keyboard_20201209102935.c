#include "keyboard.h"

static void keyboard_intr_handler()
{
    Putchar('G');
    readb(0x60);        //读输出缓冲寄存器
    return ;
}

void keyboard_init()
{
    Puts("keyboard init start!\n");
    register_intr_handler(0x21, keyboard_intr_handler);
    Puts("keyboard init done!\n");
}