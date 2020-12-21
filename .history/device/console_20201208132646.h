#ifndef __DEVICE_CONSOLE_H__
#define __DEVICE_CONSOLE_H__

#include "../lib/kernel/print.h"
#include "../kernel/lock.h"
#include "../kernel/thread.h"
#include "../lib/kernel/stdint.h"



void console_init();

void console_acquire();
void console_release();
void CPuts(char *str);
void CPutint(uint32_t num);
void CPutchar(uint8_t ch);
#endif