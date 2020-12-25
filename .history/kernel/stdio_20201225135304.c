#include "stdio.h"
#define va_arg(ap, t) *((t*)(ap += 4))	  // ap指向下一个参数并返回其值


//整形转字符串
//只传一维指针的话替换完字符串后指针实际上不会变
static void itoa(uint32_t value, char **buf, int base)
{
    char temp[1024] = {};
    int index = 0;
    //Putint(value);
    while(value)
    {
        uint32_t x = value % base;
        value = value / base;
        if(x < 10)
        {
            temp[index++] = x + '0';

        }
        else
        {
            temp[index++] = x - 10 + 'A';
        }
    }
  
    for(index = index-1; index >= 0; index --)
    {
        **buf = temp[index];
        //Putchar(temp[index]);
        (*buf)++;
    }
    
}





//制定一个str（buffer），一个format，一个参数列表
//将format按参数列表展开存到buffer
uint32_t vsprintf(char *str, const char *format, char* ap)
{
    char *buf = str;
    const char *ptr = format;
    char c = *ptr;
    uint32_t object_int;
    char * object_str;
    while(c)
    {
        if(c != '%')
        {
            *buf = c;
            buf ++;
            ptr++;
            c = *ptr;
            continue;
        }
        //跳过%
        ptr++;
        c = *ptr;
        switch(c)
        {
        case 'x':   
            ap += 4;                //下一个参数地址
            object_int = *((int *)ap);
            itoa(object_int, &buf, 16);
            ptr++;
            c = *ptr;
            break;
        case 's':
            ap += 4;                //下一个参数地址
            object_str = *((char **)ap);
            strcpy(buf, object_str);
            buf += strlen(object_str);
            ptr++;
            c = *ptr;
            break;
        case 'c':
            ap += 4;                //下一个参数地址
            object_int = *((char *)ap);
            *buf = object_int;
            buf++; ptr++;
            c = *ptr;
            break;
        case 'd':
            ap += 4;                //下一个参数地址
            object_int = *((int *)ap);
            if(object_int < 0)
            {
                object_int = -object_int;
                *buf = '-';
                buf++;
            }
            itoa(object_int, &buf, 10);
            ptr++;
            c = *ptr;
            break;

        }
    }
    return strlen(str);
}

uint32_t printf(const char *format, ...)
{
   char buf[1024] = {0};	       // 用于存储拼接后的字符串
   vsprintf(buf, format, (char *)&format); //传指针的地址  
   return write(buf); 
}

uint32_t sprintf(char *buf, const char *format, ...)
{
    return vsprintf(buf, format, (char *)&format);
}