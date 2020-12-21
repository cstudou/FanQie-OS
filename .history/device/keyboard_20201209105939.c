#include "keyboard.h"

#define ESC             '\033'
#define BACKSPACE       '\b'
#define TAB             '\t'
#define ENTER           '\r'
#define DELETE          '\177'

//不可见字符
#define CTRL_L          0
#define CTRL_R          0
#define SHIFT_L         0
#define SHIFT_R         0
#define ALT_L           0
#define ALT_R           0
#define CAPSLOCK        0

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