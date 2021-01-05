#include "fork.h"

uint32_t sys_fork()
{
    //struct TaskStruct *parent = running_thread();
    //struct TaskStruct *child = get_kernel_page(1);
    //if(child == NULL)
    {
    //    return -1;
    }
    Puts("is here");
    //copy_process(child, parent);
    //list_push_back(&thread_ready_list, &child->thread_tag);
    //list_push_back(&thread_all_list, &child->all_thread_tag);
    return 0;//child->pid;

}