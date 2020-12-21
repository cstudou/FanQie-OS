#ifndef __FANQIE_LOCK_H__
#define __FANQIE_LOCK_H__
#include "../lib/kernel/stdint.h"
#include "../lib/kernel/list.h"
#include "./kernel/interrupt.h"
#include "thread.h"

struct Semaphore
{
    uint8_t value;                  //信号量初值
    struct List semaphore_list;     //在此信号量上阻塞的所有线程
};

struct Lock
{
    struct TaskStruct *task;        //锁的持有者
    struct Semaphore semaphore;     //信号量
    uint32_t apply_count;           //申请次数，根据申请次数释放锁
};
#endif