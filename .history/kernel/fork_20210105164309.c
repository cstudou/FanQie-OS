#include "fork.h"
#include "process.h"
//复制pcb到子进程
/*
int32_t copy_pcb(struct TaskStruct *child, struct TaskStruct *parent)
{
    memcpy(child, parent, 4096);
    //child->pid = fork_pid();        //新的pid
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
    
    //memcpy(bitmap, child->user_addr.virtual_addr_bitmap.bits, page_cnt*4096);
    //child->user_addr.virtual_addr_bitmap.bits = bitmap;
    //更新名字
    //strcat(child->name,"_fork");
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
int32_t build_child_stack(struct TaskStruct *child)
{
    struct InterruptStack *stack = (struct InterruptStack *) \
        ((uint32_t)child + 4096 - sizeof(struct InterruptStack));
    stack->eax = 0;
    uint32_t *ret_thread_stack = (uint32_t *)stack - 1;
    uint32_t *ebp_in_stack = (uint32_t *)stack -5;
    *ret_thread_stack = (uint32_t)intr_exit;
    child->self_stack = ebp_in_stack;
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
    //child->page_addr = create_process_page_dir();
    //copy_body_stack(child, parent, buf);
    //build_child_stack(child);
    //sys_free(buf);
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
    //list_push_back(&thread_ready_list, &child->thread_tag);
    //list_push_back(&thread_all_list, &child->all_thread_tag);
   // return child->pid;

}
*/


/* 将父进程的pcb、虚拟地址位图拷贝给子进程 */
static int32_t copy_pcb_vaddrbitmap_stack0(struct TaskStruct* child_thread, struct TaskStruct* parent_thread) {
/* a 复制pcb所在的整个页,里面包含进程pcb信息及特级0极的栈,里面包含了返回地址, 然后再单独修改个别部分 */
   memcpy(child_thread, parent_thread, 4096);
   child_thread->pid = fork_pid();
   child_thread->total_time = 0;
   child_thread->task_status = READY;
   child_thread->clock = child_thread->priority;   // 为新进程把时间片充满
   child_thread->parent_pid = parent_thread->pid;
   child_thread->thread_tag.prev = child_thread->thread_tag.next = NULL;
   child_thread->all_thread_tag.prev = child_thread->all_thread_tag.next = NULL;
   memory_desc_init(child_thread->user_memory_desc);
/* b 复制父进程的虚拟地址池的位图 */
  
   return 0;
}


/* 拷贝父进程本身所占资源给子进程 */
static int32_t copy_process(struct TaskStruct* child_thread, struct TaskStruct* parent_thread) {
   /* 内核缓冲区,作为父进程用户空间的数据复制到子进程用户空间的中转 */
   void* buf_page = get_kernel_page(1);
   if (buf_page == NULL) {
      return -1;
   }

   /* a 复制父进程的pcb、虚拟地址位图、内核栈到子进程 */
   if (copy_pcb_vaddrbitmap_stack0(child_thread, parent_thread) == -1) {
      return -1;
   }

   /* b 为子进程创建页表,此页表仅包括内核空间 */
   
   return 0;
}

/* fork子进程,内核线程不可直接调用 */
uint32_t sys_fork(void) {
   struct task_struct* parent_thread = running_thread();
   struct task_struct* child_thread = get_kernel_page(1);    // 为子进程创建pcb(task_struct结构)
   if (child_thread == NULL) {
      return -1;
   }
   //ASSERT(INTR_OFF == intr_get_status() && parent_thread->pgdir != NULL);

   if (copy_process(child_thread, parent_thread) == -1) {
      return -1;
   }

   /* 添加到就绪线程队列和所有线程队列,子进程由调试器安排运行 */
   return 10;
}
