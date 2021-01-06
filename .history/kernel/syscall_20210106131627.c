#include "syscall.h"
extern void cls_screen();
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
uint32_t sys_read(void *buf)
{
   CPutchar('*');
   char *read_buf = (char *)buf;
   uint32_t index = 0;
   while(index < 1)
   {
      *read_buf = ioqueue_getchar(&keyboard_queue);
      index++; read_buf++;
   }
   return (index==0) ? 0x3f3f3f3f : (int)index;
}
void sys_putchar(char c)
{
   CPutchar(c);
}

/* 初始化系统调用 */
void syscall_init(void) 
{
   Puts("syscall_init start\n");
   syscall_table[SYSCALL_GETPID] = sys_getpid;
   syscall_table[SYSCALL_WRITE] = sys_write;
   syscall_table[SYSCALL_MALLOC] = sys_malloc;
   syscall_table[SYSCALL_FREE] = sys_free;
   syscall_table[SYSCALL_FORK] = sys_fork;
   syscall_table[SYSCALL_PUTCHAR] = sys_putchar;
   syscall_table[SYSCALL_CLEAR] = cls_screen;
   syscall_table[SYSCALL_READ] = sys_read;
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

uint32_t read(void *buf, uint32_t cnt)
{
   return _syscall1(SYSCALL_READ, buf);
}
void putchar(char c)
{
   _syscall1(SYSCALL_PUTCHAR, c);
}
void clear()
{
   _syscall0(SYSCALL_CLEAR);
}