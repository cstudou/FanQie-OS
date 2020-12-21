#ifndef __FANQIE_LOCK_H__
#define __FANQIE_LOCK_H__
#include "../lib/kernel/stdint.h"
#include "../lib/kernel/list.h"
#include "interrupt.h"
#include "thread.h"
/*
    利用信号量加锁，需要两个结构体
    1.semaphore结构体，保存信号量值和在这个信号量上阻塞的线程
    2.lock结构体，其拥有一个信号量，保存拥有锁的线程和该线程申请次数
*/
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




//初始化信号量
void sem_init(struct Semaphore *sem, uint8_t value);
//初始化锁
void lock_init(struct Lock *lock);
//信号量减1
void sem_wait(struct Semaphore *sem);

//信号量+1
void sem_post(struct Semaphore *sem);

void lock_acquire(struct Lock* lock);
//释放锁
void lock_release(struct Lock *lock);
#endif