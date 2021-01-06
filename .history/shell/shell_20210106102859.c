#include "shell.h"


char dir_buf[64];
void print_head()
{
    printf("[root@fanqie %s]$ ", dir_buf);
}