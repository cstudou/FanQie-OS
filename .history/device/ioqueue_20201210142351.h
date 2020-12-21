#ifndef __DEVICE_IOQUEUE_H__
#define __DEVICE_IOQUEUE_H__

#include "../lib/kernel/stdint.h"
#include "../kernel/thread.h"
#include "../kernel/lock.h"

#define BUFFERSIZE 128
//环形缓冲区
struct IoQueue
{
    struct Lock lock;               //每次操作都要申请锁
    struct TaskStruct *product;
    struct TaskStruct *consum;
    char buf[BUFFERSIZE];
    int32_t head;
    int32_t tail;
};

int ioqueue_full(struct IoQueue *qu);
int ioqueue_empty(struct IoQueue *qu);
char ioqueue_getchar(struct IoQueue *qu);
void ioqueue_putchar(struct IoQueue *qu, char str);
void ioqueue_init(struct IoQueue *qu);
#endif