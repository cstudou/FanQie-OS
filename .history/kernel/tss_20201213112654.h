#ifndef __KERNEL_TSS_H__
#define __KERNEL_TSS_H__

#include "../lib/kernel/stdint.h"
#include "thread.h"
#include "global.h"

struct Tss
{
    uint32_t backlink;
    uint32_t* esp0;
    uint32_t ss0;
    uint32_t* esp1;
    uint32_t ss1;
    uint32_t* esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t (*eip) (void); 
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint32_t trace;             //ldt选择子
    uint32_t io_base;           //IO位图在TSS中的偏移地址
};

//段描述符结构体
struct GdtDesc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t attr_low;
    uint8_t limit_high_attr_high;
    uint8_t base_high;


};

void tss_init();
#endif