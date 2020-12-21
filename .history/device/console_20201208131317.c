#include "console.h"

static struct Lock console_lock;

void console_init()
{
    lock_init(&console_init);
}