#include "thread.h"
#define PAGE_SIZE 4096

struct TaskStruct *main_thread_pcb;      //主线程pcb
struct List thread_ready_list;          //就绪队列
struct List thread_all_list;            //所有任务队列
static struct ListPtr* thread_tag;      //保存队列中的线程节点
static void kernel_thread(void (*function)(void *), void *arg)
{
    On_interrupt();
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
    if(pthread == main_thread_pcb)
    {
        pthread->task_status = RUNNING;         //运行状态
    }
    else
    {
        pthread->task_status = READY;
    }
    
    pthread->self_stack = (uint32_t *)((uint32_t)pthread + 4096);
    pthread->stack_margin = 0x19977991;     //魔数
    pthread->clock = priority;
    pthread->total_time = 0;
    pthread->page_addr = (void *)0;
}

//taskstruct:栈顶，状态，优先级，名字，边缘魔数(放在边界，检查是否溢出)
struct TaskStruct* thread_start(char *name, int priority, void (* function)(void *), void *args)
{
    struct TaskStruct * thread_pcb = get_kernel_page(1);
    init_thread(thread_pcb, name, priority);
    thread_create(thread_pcb, function, args);
    if(!list_find(&thread_ready_list, &thread_pcb->thread_tag))
    {
        list_push_back(&thread_ready_list, &thread_pcb->thread_tag);
    }
    if(!list_find(&thread_all_list, &thread_pcb->thread_tag))
    {
        list_push_back(&thread_all_list, &thread_pcb->all_thread_tag);
    }
    //通用约束g，即内存或寄存器都可以。
    //ret 会把栈顶的数据作为返回地址送上处理器的EIP 寄存器。
   
   /* asm volatile("movl %0, %%esp; \
            pop %%ebp; \
            pop %%ebx; \
            pop %%edi; \
            pop %%esi; \
            ret"::"g"(thread_pcb->self_stack) : "memory");
            */
    return thread_pcb;
}

//获取当前线程PCB指针
struct TaskStruct* running_thread()
{
    uint32_t esp;
    asm volatile("movl %%esp, %0":"=g"(esp));
    return (struct TaskStruct *)(esp & 0xfffff000);
}
