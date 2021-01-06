#include "shell.h"


char dir_buf[64];
void print_head()
{
    printf("[root@fanqie %s]$ ", dir_buf);
}

void readline(char *buf, int32_t cnt)
{

}

void shell()
{
    dir_buf[0] = '/';
    while(1)
    {
        print_head();
    }
}