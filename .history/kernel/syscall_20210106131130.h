#ifndef __KERNEL_SYSCALL_H__
#define __KERNEL_SYSCALL_H__
#include "../lib/kernel/stdint.h"
#include "../lib/kernel/print.h"
#include "thread.h"
#include "../device/console.h"
#include "fork.h"
#include "../device/ioqueue.h"
#define SYSCALL_GETPID 0
#define SYSCALL_WRITE  1
#define SYSCALL_MALLOC 2
#define SYSCALL_FREE   3
#define SYSCALL_FORK   4
#define SYSCALL_READ   5
#define SYSCALL_PUTCHAR 5
#define SYSCALL_CLEAR  7
extern struct IoQueue keyboard_queue;
//eax 为子功能号，ebx 保存第 1 个参数，ecx 保存第 2 个参数，edx 保存第 3 个参数
#define _syscall0(NUM)                      \
    ({                                      \
        int ret;                            \
        asm volatile("int $0x80":"=a"(ret):"a"(NUM):"memory"); \
        ret;                                \
    })

#define _syscall1(NUM, ARG1)                \
    ({                                      \
        int ret;                            \
        asm volatile("int $0x80":"=a"(ret):"a"(NUM),"b"(ARG1):"memory"); \
        ret;                                \
    })
#define _syscall2(NUM, ARG1, ARG2)    \
    ({                                      \
        int ret;                            \
        asm volatile("int $0x80":"=a"(ret):"a"(NUM),"b"(ARG1),"c"(ARG2):"memory"); \
        ret;                                \
    })
#define _syscall3(NUM, ARG1, ARG2, ARG3)    \
    ({                                      \
        int ret;                            \
        asm volatile("int $0x80":"=a"(ret):"a"(NUM),"b"(ARG1),"c"(ARG2),"d"(ARG3):"memory"); \
        ret;                                \
    })

uint32_t getpid(void);
void syscall_init(void);
uint32_t sys_getpid(void);
uint32_t write(char *str);
uint32_t read(void *buf, uint32_t cnt);
void *malloc(uint32_t size);
void free(void *ptr);
uint32_t fork(void);
void putchar(char c);
#endif