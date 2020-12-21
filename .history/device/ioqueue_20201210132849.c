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
    *task = running_thread();
    thread_block(BLOCKED);
}

static void ioqueue_wakeup(struct TaskStruct **task)
{
    thread_unblock(*task);
    *task = (void *)0;
}


char ioqueue_getchar(struct IoQueue *qu)
{
    while(!ioqueue_empty(qu))
    {
        lock_acquire(&qu->lock);

        lock_release(&qu->lock);
    }
}