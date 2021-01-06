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
    
    while(read(index, 1)!=0x3f3f3f3f && (index-buf)<cnt)
    {
        switch(*index)
        {
        case '\n':
        case '\r':
            *index = '\0';
         //   putchar('#');
            return ;    
        case '\b':
            if(buf[0] != '\b')
            {
           //     putchar('\b');
                index--;
                break;
            }
        default:
         //   putchar(*index);
            index++;
        }
    }
    //while(1);
    printf("\ncannot find enter_key\n");
}

void shell()
{
    dir_buf[0] = '/';
    while(1)
    {
        print_head();
        memset(cmd, 0, 128);
        //readline(cmd, 128);         //从键盘缓冲区读
        char *temp[2] = {};
        read(temp, 1);
        
    }
}