#include "fork.h"
#include "process.h"
//复制pcb到子进程

int32_t copy_pcb(struct TaskStruct *child, struct TaskStruct *parent)
{
    memcpy(child, parent, 4096);
    child->pid = fork_pid();        //新的pid
    child->total_time = 0;
    child->task_status = READY;
    child->clock = child->priority;
    child->parent_pid = parent->pid;
    child->thread_tag.prev = child->thread_tag.next = (void *)0;
    child->all_thread_tag.next = child->all_thread_tag.prev = (void *)0;
    memory_desc_init(child->user_memory_desc);

    //复制父进程虚拟地址位图
    uint32_t page_cnt = (((0xc0000000 - USER_ADDR_START) / 4096 / 8) + 4096 - 1) / 4096;

    void *bitmap = get_kernel_page(page_cnt);

    Putint(child->pid);
    memcpy(bitmap, child->user_addr.virtual_addr_bitmap.bits, page_cnt*4096);
    child->user_addr.virtual_addr_bitmap.bits = bitmap;
    //更新名字
    strcat(child->name,"_fork");
    return 0;

}

//复制子进程进程体、用户栈
void copy_body_stack(struct TaskStruct *child, struct TaskStruct *parent, void* buf)
{
    uint8_t *bitmap_bits = parent->user_addr.virtual_addr_bitmap.bits;
    uint32_t bitmap_len = parent->user_addr.virtual_addr_bitmap.bitmap_len;
    uint32_t vaddr_start = parent->user_addr.virtual_addr_start;

    uint32_t index_bytes = 0, index_bits = 0, vaddr = 0;
    //在父进程中查找有数据的页
    while(index_bytes < bitmap_len)
    {
        if(bitmap_bits[index_bytes])
        {
            index_bits = 0;
            while(index_bits < 8)
            {
                if((1<<index_bits) & bitmap_bits[index_bytes])
                {
                    vaddr = (index_bytes * 8 + index_bits) * 4096 + vaddr_start;
                    //复制数据
                    memcpy(buf, (void *)vaddr, 4096);
                    //切换到子进程
                    process_activate(child);
                    get_page_fork(USERPOOL, vaddr);
                    memcpy((void *)vaddr, buf, 4096);
                    process_activate(parent);
                }
                index_bits++;
            }
        }
        index_bytes++;
    }
}

//为子进程构建栈
int32_t build_child_stack(struct TaskStruct *child_thread)
{   /* 获取子进程0级栈栈顶 */
   struct InterruptStack* intr_0_stack = (struct intr_stack*)((uint32_t)child_thread + 4096 - sizeof(struct InterruptStack));
   /* 修改子进程的返回值为0 */
   intr_0_stack->eax = 0;

/* b 为switch_to 构建 struct thread_stack,将其构建在紧临intr_stack之下的空间*/
   uint32_t* ret_addr_in_thread_stack  = (uint32_t*)intr_0_stack - 1;

   /***   这三行不是必要的,只是为了梳理thread_stack中的关系 ***/
   uint32_t* esi_ptr_in_thread_stack = (uint32_t*)intr_0_stack - 2; 
   uint32_t* edi_ptr_in_thread_stack = (uint32_t*)intr_0_stack - 3; 
   uint32_t* ebx_ptr_in_thread_stack = (uint32_t*)intr_0_stack - 4; 
   /**********************************************************/

   /* ebp在thread_stack中的地址便是当时的esp(0级栈的栈顶),
   即esp为"(uint32_t*)intr_0_stack - 5" */
   uint32_t* ebp_ptr_in_thread_stack = (uint32_t*)intr_0_stack - 5; 

   /* switch_to的返回地址更新为intr_exit,直接从中断返回 */
   *ret_addr_in_thread_stack = (uint32_t)intr_exit;

   /* 下面这两行赋值只是为了使构建的thread_stack更加清晰,其实也不需要,
    * 因为在进入intr_exit后一系列的pop会把寄存器中的数据覆盖 */
   *ebp_ptr_in_thread_stack = *ebx_ptr_in_thread_stack =\
   *edi_ptr_in_thread_stack = *esi_ptr_in_thread_stack = 0;
   /*********************************************************/

   /* 把构建的thread_stack的栈顶做为switch_to恢复数据时的栈顶 */
   child_thread->self_stack = ebp_ptr_in_thread_stack;	    
   return 0;
}

//拷贝父进程资源给子进程
int32_t copy_process(struct TaskStruct* child, struct TaskStruct *parent)
{
    void *buf = get_kernel_page(1);
    if(!buf)
    {
        return -1;
    }
    if(copy_pcb(child, parent) == -1)
    {
        return -1;
    }
    child->page_addr = create_process_page_dir();
    copy_body_stack(child, parent, buf);
    build_child_stack(child);
    paddr_remove(KERNELPOOL, buf, 1);
    return 0;
}

uint32_t sys_fork()
{
    struct TaskStruct *parent = running_thread();
    struct TaskStruct *child = get_kernel_page(1);
    if(child == NULL)
    {
        return -1;
    }

    copy_process(child, parent);
    list_push_back(&thread_ready_list, &child->thread_tag);
    list_push_back(&thread_all_list, &child->all_thread_tag);
    return child->pid;

}
