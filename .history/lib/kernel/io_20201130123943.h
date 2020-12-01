#ifndef __FANQIE_IO_H__
#define __FANQIE_IO_H__
#include "stdint.h"

static inline void outb(uint16_t port, uint8_t data)
{
    asm("out %b0, %w1"::"a"(data), "Nd"(port));        //N:表示操作数为 0~255 之间的立即数
}
#endif