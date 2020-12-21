#include "ioqueue.h"

void ioqueue_init(struct IoQueue *qu)
{
    qu->head = qu->tail = 0;
    qu->consum = qu->product = (void *)0;
    lock_init(&qu->lock);
}

int ioqueue_full(struct IoQueue *qu)
{
    if(((qu->head + 1) % BUFFERSIZE) == qu->tail)
    {
        return 1;
    }
    return 0;
}

int ioqueue_empty(struct IoQueue *qu)
{
    if(qu->tail == qu->head)
    {
        return 1;
    }
    return 0;
}


//当前生产者或者消费者等待
static void ioqueue_wait(struct TaskStruct **task)
{
    *task = running_thread();   //将当前线程记录
    thread_block(BLOCKED);
}

static void ioqueue_wakeup(struct TaskStruct **task)
{
    thread_unblock(*task);
    *task = (void *)0;
}


char ioqueue_getchar(struct IoQueue *qu)
{
    while(ioqueue_empty(qu))
    {
        lock_acquire(&qu->lock);
        ioqueue_wait(&qu->consum);
        lock_release(&qu->lock);
    }
    char str = qu->buf[qu->head];
    qu->tail ++;
    if(qu->product != (void *)0)
    {
        ioqueue_wakeup(&qu->product);
    }
    return str;
}

void ioqueue_putchar(struct IoQueue *qu, char str)
{
    while(ioqueue_full(qu))
    {
        lock_acquire(&qu->lock);
        ioqueue_wait(&qu->consum);
        lock_release(&qu->lock);
    }
    qu->buf[qu->head] = str;
    qu->head = (qu->head + 1) % BUFFERSIZE;
    if(qu->consum != (void *)0)
    {
        ioqueue_wakeup(&qu->consum);
    }
}