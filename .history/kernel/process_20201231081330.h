#ifndef __KERNEL_PROCESS_H__
#define __KERNEL_PROCESS_H__
#include "thread.h"
#include "../lib/kernel/memory.h"
#include "tss.h"

#define USER_ADDR_START 0x8048000
#define DEFAULT_PROCESS_PRIORITY 3
void process_activate(struct TaskStruct *task);
void create_user_process(void *filename, char *name);
extern struct List thread_ready_list;          //就绪队列
extern struct List thread_all_list;            //所有任务队列，共创建了多少线程
#endif