#include "lock.h"

//初始化信号量
void sem_init(struct Semaphore *sem, uint8_t value)
{
    sem->value  = value;
    list_init(&sem->semaphore_list);
}

//初始化锁
void lock_init(struct Lock *lock)
{
    lock->task = (void *)0;
    lock->apply_count = 0;
    sem_init(&lock->semaphore, 1);    //锁的信号量初始化为1
}

//信号量减1
void sem_wait()
{
    struct InterruptSta
}

//信号量+1
void sem_post()
{

}