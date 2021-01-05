#include "fork.h"

//复制pcb到子进程
int32_t copy_pcb(struct TaskStruct *child, struct TaskStruct *parent)
{
    memcpy(child, parent, 4096);
}