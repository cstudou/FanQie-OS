#ifndef __FANQIE_THREAD_H__
#define __FANQIE_THREAD_H__
#include "../lib/kernel/stdint.h"
enum ThreadStatus
{
    RUNNING, READY, BLOCKED, WAITTING, HANGING, DIED    
};

//保存中断时的上下文
struct InterruptStack
{
    //中断向量号
    uint32_t vector;
    //8个通用寄存器
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_copy;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    //4个段寄存器
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    //由 cpu 从低特权级进入高特权级时压入
    uint32_t error_code;
    void (*eip)(void);
    uint32_t cs;
    uint32_t eflags;
    void *esp;
    uint32_t ss;
};

struct ThreadStack
{
    uint32_t ebp;
    uint32_t enx;
    uint32_t edi;
    uint32_t esi;
    void (*eip)(void (*func)(void *), void * func_arg);

    void (*unused_retaddr); 
    void (*function)(void *); 
    void* func_arg;
};
#endif