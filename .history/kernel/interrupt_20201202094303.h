#ifndef __FANQIE_INTERRUPT_H__
#define __FANQIE_INTERRUPT_H__

#include "../lib/kernel/stdint.h"
#include "../lib/kernel/io.h"
#include "global.h"
#include "../lib/kernel/print.h"

enum intertupt_status
{
    InterruptOff,
    InterruptOn
};

void idt_init();
#endif