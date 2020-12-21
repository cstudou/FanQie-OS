#ifndef __DEVICE_KEYBOARD_H__
#define __DEVICE_KEYBOARD_H__
#include "ioqueue.h"
#include "../lib/kernel/print.h"
#include "../lib/kernel/io.h"
#include "../kernel/interrupt.h"
extern struct IoQueue keyboard_queue;
void keyboard_init();
#endif