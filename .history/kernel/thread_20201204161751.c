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

void init_thread(struct TaskStruct* pthread, char *name, int priority)
{
    memset(pthread, 0, sizeof(struct TaskStruct));
    pthread->priority = priority;
    strcpy(pthread->name, name);
    pthread->task_status = RUNNING;         //运行状态
    pthread->self_stack = (uint32_t *)((uint32_t)pthread + 4096);
    pthread->stack_margin = 0x19870916;     //魔数
}

//taskstruct:栈顶，状态，优先级，名字，边缘魔数
struct TaskStruct* thread_start(char *name, int priority, void (* function)(void *), void *args)
{
    struct TaskStruct * task_struct = get_kernel_page(1);
    init_thread(task_struct, name, priority);
    thread_create(task_struct, function, args);
}


