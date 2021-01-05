#include "syscall.h"
void* syscall_table[32];

/* 返回当前任务的pid */
uint32_t sys_getpid(void) {
   struct TaskStruct *cur = running_thread();
   return cur->pid;
}
uint32_t sys_write(char *str)
{
   CPuts(str);
   return strlen(str);
}

/* 初始化系统调用 */
void syscall_init(void) 
{
   Puts("syscall_init start\n");
   syscall_table[SYSCALL_GETPID] = sys_getpid;
   syscall_table[SYSCALL_WRITE] = sys_write;
   syscall_table[SYSCALL_MALLOC] = sys_malloc;
   syscall_table[SYSCALL_FREE] = sys_free;
   //syscall_table[SYSCALL_FORK] = sys_fork;
   Puts("syscall_init done\n");
}
/* 返回当前任务pid */
uint32_t getpid() 
{
   return _syscall0(SYSCALL_GETPID);
}

uint32_t write(char *str)
{
   return _syscall1(SYSCALL_WRITE ,str);
}

void *malloc(uint32_t size)
{
   return (void *)_syscall1(SYSCALL_MALLOC, size);
}
void free(void *ptr)
{
   _syscall1(SYSCALL_FREE, ptr);
}

uint32_t fork()
{
   return _syscall0(SYSCALL_FORK);
}