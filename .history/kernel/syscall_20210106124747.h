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

/* 无参数的系统调用 */
#define _syscall0(NUMBER) ({				       \
   int retval;					               \
   asm volatile (					       \
   "int $0x80"						       \
   : "=a" (retval)					       \
   : "a" (NUMBER)					       \
   : "memory"						       \
   );							       \
   retval;						       \
})

/* 一个参数的系统调用 */
#define _syscall1(NUMBER, ARG1) ({			       \
   int retval;					               \
   asm volatile (					       \
   "int $0x80"						       \
   : "=a" (retval)					       \
   : "a" (NUMBER), "b" (ARG1)				       \
   : "memory"						       \
   );							       \
   retval;						       \
})

/* 两个参数的系统调用 */
#define _syscall2(NUMBER, ARG1, ARG2) ({		       \
   int retval;						       \
   asm volatile (					       \
   "int $0x80"						       \
   : "=a" (retval)					       \
   : "a" (NUMBER), "b" (ARG1), "c" (ARG2)		       \
   : "memory"						       \
   );							       \
   retval;						       \
})

/* 三个参数的系统调用 */
#define _syscall3(NUMBER, ARG1, ARG2, ARG3) ({		       \
   int retval;						       \
   asm volatile (					       \
      "int $0x80"					       \
      : "=a" (retval)					       \
      : "a" (NUMBER), "b" (ARG1), "c" (ARG2), "d" (ARG3)       \
      : "memory"					       \
   );							       \
   retval;						       \
})

uint32_t getpid(void);
void syscall_init(void);
uint32_t sys_getpid(void);
uint32_t write(char *str);
int read(void *buf, uint32_t cnt);
void *malloc(uint32_t size);
void free(void *ptr);
uint32_t fork(void);
void putchar(char c);
#endif