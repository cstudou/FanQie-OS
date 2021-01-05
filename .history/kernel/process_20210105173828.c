#include "process.h"

extern void intr_exit(void);


/*
    内存布局
    ｜———————————————————————｜
    ｜命令行参数和环境变量      ｜
    ｜栈                     ｜
    ｜                       ｜
    ｜堆                     ｜
    ｜未初始化数据bss          ｜
    ｜初始化数据data           ｜
    ｜代码段test              ｜
    ｜———————————————————————｜


*/

//构建用户进程初始信息上下文
void start_process(void *filename)
{
    void *func = filename;
    struct TaskStruct *task = running_thread();         //进程基于线程实现
    task->self_stack += sizeof(struct ThreadStack);     //初始化完成后，指针预留了中断上下文与线程上下文
    struct InterruptStack *intr_stack = (struct InterruptStack *)task->self_stack;
    //8个通用寄存器
    
    intr_stack->edi = intr_stack->esi = intr_stack->ebp = intr_stack->esp_copy = 0;
    intr_stack->ebx = intr_stack->edx = intr_stack->ecx = intr_stack->eax = 0;
    //用户进程不能访问显存
    intr_stack->gs = 0;

    intr_stack->ds = intr_stack->es = intr_stack->fs = SELECT_U_DATA;
    intr_stack->eip = func;
    //intr_stack->eip();
    intr_stack->cs = SELECT_U_CODE;
    intr_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);    //开中断，特权级是0
    //(0xc0000000-1)是用户空间的最高地址，0xc0000000~0xffffffff 是内核空间
    //分配一页，0x1000为4kb，当栈
    intr_stack->esp = (void *)((uint32_t)get_page(USERPOOL, (0xc0000000 - 0x1000)) + 4096);
    intr_stack->ss = SELECT_U_DATA;
    //CPuts("mamsdauhfkasnfkads\n");
    asm volatile("movl %0, %%esp; jmp intr_exit"::"g"(intr_stack):"memory");
    //CPuts("mamsdauhfkasnfkads\n");
}


//激活页表
void page_dir_activate(struct TaskStruct *task)
{
    uint32_t addr = 0x100000;
    if(task->page_addr != (void *)0)
    {
        //用户态进程有页目录
        addr = addr_v2p((uint32_t)task->page_addr);
    }
    asm volatile("movl %0, %%cr3" :: "r"(addr) : "memory");
}

void process_activate(struct TaskStruct *task)
{
    page_dir_activate(task);
    if(task->page_addr)
    {
        update_tss_esp(task);   //更新0特权级栈
    }
}

//创建页目录表
//成功返回虚拟地址失败返回NULL
uint32_t* create_process_page_dir()
{
    // 用户进程的页表不能让用户直接访问到，所以在内核空间来申请 
    uint32_t *page_dir = get_kernel_page(1);
    if(page_dir == (void *)0)
    {
        return (void *)0;
    }
    //从内核页目录768开始赋值
    memcpy((uint32_t *)((uint32_t)page_dir + 0x300*4), (uint32_t *)(0xfffff000+0x300*4), 1024);
    //得到物理地址
    uint32_t new_page_dir = addr_v2p((uint32_t)page_dir);
    //构建页目录表项
    page_dir[1023] = new_page_dir | PG_US_U | PG_RW_W | PG_P_1;
    return page_dir;
}


//创建用户进程虚拟地址位图
void create_process_bitmap(struct TaskStruct *task)
{
    //大部分可执行程序的“Entry point address”都是在 0x8048000 附近
    task->user_addr.virtual_addr_start = USER_ADDR_START;
    uint32_t bitmap_cnt = (((0xc0000000 - USER_ADDR_START) / 4096 / 8) + 4096 - 1) / 4096;
    //申请内存
    task->user_addr.virtual_addr_bitmap.bits = get_kernel_page(bitmap_cnt);
    task->user_addr.virtual_addr_bitmap.bitmap_len = (0xc0000000 - USER_ADDR_START) / 4096 / 8;
    //初始化位图
    bitmap_init(&task->user_addr.virtual_addr_bitmap);
}

//此函数创建用户进程
void create_user_process(void *filename, char *name)
{
    //内核维护进程信息
    struct TaskStruct *thread = get_kernel_page(1);
    init_thread(thread, name, DEFAULT_PROCESS_PRIORITY);
    create_process_bitmap(thread);              //创建进程位图
    memory_desc_init(thread->user_memory_desc);
    thread_create(thread, start_process, filename);

    Puts("is here");

    thread->page_addr = create_process_page_dir();
    enum InterruptStatus status = Off_interrupt();
    list_push_back(&thread_ready_list, &thread->thread_tag);
    list_push_back(&thread_all_list, &thread->all_thread_tag);
    set_InterruptStatus(status);
}