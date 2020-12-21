#ifndef __KERNEL_PROCESS_H__
#define __KERNEL_PROCESS_H__
#include "thread.h"
#include "../lib/kernel/memory.h"
#include "tss.h"
#define USER_ADDR_START 0x8048000
#define DEFAULT_PROCESS_PRIORITY 3
void process_activate(struct TaskStruct *task);
#endif