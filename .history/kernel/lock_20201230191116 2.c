#include "lock.h"
#include "thread.h"
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
void sem_wait(struct Semaphore *sem)
{
    enum InterruptStatus status = Off_interrupt();  //关中断保证原子操作
    while(!sem->value)
    {
        //锁被持有
        struct TaskStruct *task = running_thread();
        //加入阻塞队列
        if(!list_find(&sem->semaphore_list, &task->thread_tag))
        {
            list_push_back(&sem->semaphore_list, &task->thread_tag);
            thread_block(BLOCKED);      //锁住线程
        }
    }
    sem->value --;
    printf("%d\n", sem_value);
    set_InterruptStatus(status);

}

//信号量+1
void sem_post(struct Semaphore *sem)
{
    enum InterruptStatus status = Off_interrupt();  //关中断保证原子操作
    if(!list_empty(&sem->semaphore_list))
    {
        //从阻塞队列中取一个线程，取消阻塞（加到准备队列头并且改状态）
        struct TaskStruct temp;
        struct ListPtr *ptr = list_pop(&sem->semaphore_list);
        struct TaskStruct *task = (struct TaskStruct *)((int)ptr - ((int)&temp.thread_tag - (int)&temp));
        thread_unblock(task);
    }
    sem->value ++;
    set_InterruptStatus(status);
}

void lock_acquire(struct Lock* lock)
{
    //加锁
    //不是当前进程的话阻塞信号量
    //否则count+1
    if(lock->task != running_thread())
    {
        //自己不持有锁
        sem_wait(&lock->semaphore);
        lock->apply_count = 1;
    }
    else
    {
        //自己持有锁
        lock->apply_count++;
    }
    
}

//释放锁
void lock_release(struct Lock *lock)
{
    if(lock->apply_count > 1)
    {
        lock->apply_count --;
        return ;
    }
    lock->task = (void *)0;
    lock->apply_count = 0;
    sem_post(&lock->semaphore);
}