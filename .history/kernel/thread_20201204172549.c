#include "thread.h"
#define PAGE_SIZE 4096

static void kernel_thread(void (*function)(void *), void *arg)
{
    function(arg);
}

//创建线程
//threadstack:ebp,ebx,edi,esi;函数运行地址，返回地址，函数指针，参数
void thread_create(struct TaskStruct *pthread, void (*function)(void *), void *arg)
{
    //self_stack初始化为线程 PCB 的 最顶端
    pthread->self_stack -= sizeof(struct InterruptStack);   //预留中断栈的位置
    pthread->self_stack -= sizeof(struct ThreadStack);      //构建线程栈
    struct ThreadStack *thread_stack = (struct ThreadStack *)pthread->self_stack;

    thread_stack->eip = kernel_thread;
    thread_stack->function = function;
    thread_stack->func_arg = arg;

    thread_stack->ebp = thread_stack->ebx = 0;
    thread_stack->esi = thread_stack->edi = 0;
}

//初始化线程pcb
void init_thread(struct TaskStruct* pthread, char *name, int priority)
{
    memset(pthread, 0, sizeof(struct TaskStruct));
    pthread->priority = priority;
    strcpy(pthread->name, name);
    pthread->task_status = RUNNING;         //运行状态
    pthread->self_stack = (uint32_t *)((uint32_t)pthread + 4096);
    pthread->stack_margin = 0x19977991;     //魔数
}

//taskstruct:栈顶，状态，优先级，名字，边缘魔数(放在边界，检查是否溢出)
struct TaskStruct* thread_start(char *name, int priority, void (* function)(void *), void *args)
{
    struct TaskStruct * task_struct = get_kernel_page(1);
    init_thread(task_struct, name, priority);
    Puts("it is ok \n");
    thread_create(task_struct, function, args);
    //通用约束g，即内存或寄存器都可以。
    //ret 会把栈顶的数据作为返回地址送上处理器的EIP 寄存器。
    asm volatile("movl %0, %%esp; \
            pop %%ebp; \
            pop %%ebx; \
            pop %%edi; \
            pop %%esi; \
            ret"::"g"(task_struct->self_stack) : "memory");
}


