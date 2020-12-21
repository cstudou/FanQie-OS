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

//通码&断码
#define SHIFT_L_ON      0x2a
#define SHIFT_R_ON      0x36
#define ALT_L_ON        0x38
#define ALT_R_ON        0xe038
#define ALT_R_OFF       0xe0b8
#define CTRL_L_ON       0x1d
#define CTRL_R_ON       0xe01d
#define CTRL_R_OFF      0xe09d
#define CAPSLOCK_ON     0x3a

//ext_scancode判断数据是否是0xe0开头
static bool ctrl_status, shift_status, alt_status, capslock_status, ext_scancode;
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