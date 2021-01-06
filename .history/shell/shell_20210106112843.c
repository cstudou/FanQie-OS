#include "shell.h"

extern void cls_screen();
char dir_buf[64];
char cmd[128];
void print_head()
{
   // cls_screen();
    printf("[root@fanqie %s]$ ", dir_buf);
}

//从键盘缓冲区读cnt字符到buf
void readline(char *buf, int32_t cnt)
{
    char *index = buf;
    
    while(read(index, 1)>0 && (index-buf)<cnt)
    {
       printf("cannot find enter_key\n");
       while(1);
    }
   // printf("cannot find enter_key\n");
}

void shell()
{
    dir_buf[0] = '/';
    while(1)
    {
        print_head();
        memset(cmd, 0, 128);
        readline(cmd, 128);         //从键盘缓冲区读
        
    }
}