#include "console.h"

static Lock console_lock;

void console_init()
{
    lock_init(&console_init);
}