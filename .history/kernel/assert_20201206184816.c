#include "assert.h"

void debug_message(char *filename, int line, const char *func, const char * condition)
{
    Off_interrupt();
    set_cursor(0);
    
    Puts("\n\n\n!!!!! error !!!!!\n"); 
    Puts("filename:");
    Puts(filename);
    Puts("\n"); 
    Puts("line:0x");
    Putint(line);
    Puts("\n"); 
    Puts("function:");
    Puts((char*)func);
    Puts("\n"); 
    Puts("condition:");
    Puts((char*)condition);Puts("\n");
    while(1);

}