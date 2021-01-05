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
                }
            }
        }
    }
}