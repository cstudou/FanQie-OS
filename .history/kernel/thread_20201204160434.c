#include "thread.h"
#define PAGE_SIZE 4096

static void kernel_thread(void (*function)(void *), void *arg)
{
    function(arg);
}

//创建线程
void thread_create(struct TaskStruct *pthread, void (*function)(void *), void *arg)
{
    pthread->self_stack -= sizeof(struct InterruptStack);
    pthread->self_stack -= sizeof(struct ThreadStack);
    struct ThreadStack *thread_stack = (struct ThreadStack *)pthread->self_stack;

    thread_stack->eip = thread_stack;
    thread_stack->function = function;
    thread_stack->func_arg = arg;
    thread_stack->ebp = thread_stack->ebx = 0;
    thread_stack->esi = thread_stack->edi = 0;
}