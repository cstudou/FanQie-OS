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