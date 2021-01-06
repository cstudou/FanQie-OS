#ifndef __KERNEL_STDIO_H__
#define __KERNEL_STDIO_H__

#include "../lib/kernel/print.h"
#include "syscall.h"
typedef char* va_list;

uint32_t printf(const char *format, ...);
uint32_t sprintf(char *buf, const char *format, ...);
void printk(const char* format, ...);
#endif