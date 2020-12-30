#ifndef __FANQIE_INTERRUPT_H__
#define __FANQIE_INTERRUPT_H__

#include "../lib/kernel/stdint.h"
#include "../lib/kernel/io.h"
#include "global.h"
#include "../lib/kernel/print.h"

enum InterruptStatus
{
    InterruptOff,
    InterruptOn
};
enum InterruptStatus Off_interrupt();
enum InterruptStatus On_interrupt();
enum InterruptStatus interrupt_get_status();
enum InterruptStatus set_InterruptStatus(enum InterruptStatus status);
void idt_init();

void register_intr_handler(uint8_t index, void *);
#endif