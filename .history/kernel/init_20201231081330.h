#ifndef __FANQIE_INIT_H__
#define __FANQIE_INIT_H__

#include "interrupt.h"
#include "../device/timer.h"
#include "../lib/kernel/memory.h"
#include "../kernel/thread.h"
#include "../device/console.h"
#include "../device/keyboard.h"
#include "tss.h"
#include "syscall.h"
#include "../device/ide.h"
void init_all();

#endif