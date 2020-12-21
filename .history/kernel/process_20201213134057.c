#include "process.h"

extern void intr_exit(void);

void start_process(void *filename)
{
    void *func = filename;
    struct Task
}