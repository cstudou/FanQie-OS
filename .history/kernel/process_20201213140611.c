#include "process.h"

extern void intr_exit(void);

void start_process(void *filename)
{
    void *func = filename;
    struct TaskStruct *task = running_thread();
    task->self_stack += sizeof(struct ThreadStack);
    struct InterruptStack *intr_stack = (struct InterruptStack *)task->self_stack;

    intr_stack->edi = intr_stack->esi = intr_stack->ebp = intr_stack->esp_copy = 0;
    intr_stack->ebx = intr_stack->edx = intr_stack->ecx = intr_stack->eax = 0;
    intr_stack->gs = 0;

    intr_stack->ds = intr_stack->es = intr_stack->fs = SELECT_U_DATA;
    intr_stack->eip = func;
    intr_stack->cs = SELECT_U_CODE;
    intr_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);
    intr_stack->esp = ((uint32_t)get_page(USERPOOL, USER_STACK_VADDR) + 4096);
    intr_stack->ss = SELECT_U_DATA;
    asm volatile("movl %0, %%esp; jmp intr_exit"::"g"(intr_stack):"memory");
}

//激活页表
void page_dir_activate(struct TaskStruct *task)
{
    uint32_t addr = 0x100000;
    if(task->page_addr != (void *)0)
    {
        //此处为用户态线程
        addr = addr_v2p(task->page_addr);
    }
}