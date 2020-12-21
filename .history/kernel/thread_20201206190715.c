#include "thread.h"
#define PAGE_SIZE 4096

struct TaskStruct *main_thread_pcb;      //主线程pcb
struct List thread_ready_list;          //就绪队列
struct List thread_all_list;            //所有任务队列，共创建了多少线程
static struct ListPtr* thread_tag;      //保存队列中的线程节点
extern void switch_to(struct TaskStruct*, struct TaskStruct *);
static void kernel_thread(void (*function)(void *), void *arg)
{
    On_interrupt(); //线程运行是由时钟中断处理函数调用任务调度器完成的，进入中断后处理器会关中断
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
    if(thread_pcb !== (void *)0)
    {
        Puts("hwll\n\n\n\n\n");
    }
    init_thread(thread_pcb, name, priority);
    thread_create(thread_pcb, function, args);
    //不在队列中就加入
    Puts("\nthread start done\n");
    if(!list_find(&thread_ready_list, &thread_pcb->thread_tag))
    {
        list_push_back(&thread_ready_list, &thread_pcb->thread_tag);
    }
    //Putint(list_size(&thread_ready_list));
    if(!list_find(&thread_all_list, &thread_pcb->thread_tag))
    {
        list_push_back(&thread_all_list, &thread_pcb->all_thread_tag);
    }
    //Putint(list_size(&thread_all_list));
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
    return (struct TaskStruct *)(esp & 0xfffff000); //取高20位，能找到一个页面
}

//创建主线程pcb
static void make_main_thread()
{
    main_thread_pcb = running_thread();
    init_thread(main_thread_pcb, "main", 31);
    //当前运行的进程必不在就绪队列
    if(!list_find(&thread_all_list, &main_thread_pcb->thread_tag))
    {
        list_push_back(&thread_all_list, &main_thread_pcb->all_thread_tag);
    }
}


void schedule()
{
    //当前线程时间片到期，换下cpu
    ASSERT(interrupt_get_status() == InterruptOff);
    struct TaskStruct *pcb = running_thread();
    if(pcb->task_status == RUNNING)
    {
        //此时为时间片到期
        //加入就绪队列
        ASSERT(!list_find(&thread_ready_list, &pcb->thread_tag));
        list_push_back(&thread_ready_list, &pcb->thread_tag);
        pcb->clock = pcb->priority;
        pcb->task_status = READY;
    }
    else
    {
        /* code */
    }
    ASSERT(!list_empty(&thread_ready_list));
    thread_tag = (void *)0;
    //弹出一个就绪线程，弹出的是pcb中thread_tag的地址
    thread_tag = list_pop(&thread_ready_list);

    //将新弹出的线程换上cpu
    //弹出来的thread_tag是结构体中的thread_tag
    struct TaskStruct temp_task;
    int length = (int)&temp_task - (int)&temp_task.thread_tag;  //地址偏移
    //new_task为pcb起始地址
    struct TaskStruct *new_task = (struct TaskStruct*)((int)thread_tag - length);
    //从pcb切换到new_task，切换后esp为新new_task
    //新的new_task 第一个元素是栈地址，弹出4个寄存器之后是函数地址
    switch_to(pcb ,new_task);
}


void thread_init()
{
    Puts("thread init start\n");
    list_init(&thread_all_list);
    list_init(&thread_ready_list);
    make_main_thread();             //将当前main函数创建为线程，在pcb上写线程信息
    Puts("thread init done\n");
}