#ifndef __KERNEL_FORK_H__
#define __KERNEL_FORK_H__
#include "../lib/kernel/stdint.h"

extern void intr_exit();
uint32_t sys_fork(void);
#endif