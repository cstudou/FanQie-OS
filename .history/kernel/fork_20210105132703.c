#include "fork.h"

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
    uint32_t page_cnt = (0xc0000000 - USER_ADDR_START) / 4096 / 8;
    void *bitmap = get_kernel_page(page_cnt);
    memcpy(bitmap, child->user_addr.virtual_addr_bitmap.bits, page_cnt*4096);
    child->user_addr.virtual_addr_bitmap.bits = bitmap;

}