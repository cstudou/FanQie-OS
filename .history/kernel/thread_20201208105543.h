#ifndef __FANQIE_THREAD_H__
#define __FANQIE_THREAD_H__
#include "../lib/kernel/stdint.h"
#include "../lib/kernel/string.h"
#include "../lib/kernel/memory.h"
#include "../lib/kernel/list.h"
//#define offset(type, member) ((int)type) 
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
    //ebp、ebx、edi、esi、和 esp 归主调函数所用，在被调函数执行完后，这 5 个寄存器的值不 该被改变
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;
    void (*eip)(void (*func)(void *), void * func_arg);   //保存运行函数的地址

    void (*unused_retaddr);     //返回地址
    void (*function)(void *);   //在线程中执行的函数
    void* func_arg;             //所需的参数
};

struct TaskStruct
{
    uint32_t *self_stack;       //进程的内核栈
    enum ThreadStatus task_status;
    uint8_t priority;
    char name[16];

    uint8_t clock;              //时间片,优先级越高时间片越长
    uint32_t total_time;        //运行总时间
    struct ListPtr thread_tag;  //线程的标签
    struct ListPtr all_thread_tag;

    uint32_t *page_addr;        //页表虚拟地址
    uint32_t stack_margin;      //栈的边界标记，用于检测栈的溢出
};


struct TaskStruct* thread_start(char *name, int priority, void (* function)(void *), void *args);
struct TaskStruct* running_thread();
void schedule();
void thread_init();

void thread_block(enum ThreadStatus status);
void thread_unblock(struct TaskStruct *thread);
#endif