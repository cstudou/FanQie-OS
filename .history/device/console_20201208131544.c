#include "console.h"

static struct Lock console_lock;

void console_init()
{
    lock_init(&console_lock);
}

void console_acquire()
{
    lock_acquire(&console_lock);
}
void console_release()
{
    lock_release(&console_lock);
}
void CPuts(char *str)
{
    console_acquire();
    Puts(str);
    console_release();
}

void CPutint(uint32_t num)
{
    console_acquire();
    Putint(num);
    console_release();    
}

void CPutchar(uint8_t ch)
{
    console_acquire();
    Putchar(ch);
    console_release();    
}