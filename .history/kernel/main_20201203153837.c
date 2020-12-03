#include "../lib/kernel/print.h"
#include "../kernel/init.h"
#include "../kernel/assert.h"

int main()
{
/*
    Putchar('\n');
    Putchar('k'); 
    Putchar('e'); 
    Putchar('r'); 
    Putchar('n'); 
    Putchar('e'); 
    Putchar('l'); 
    Putchar('\n');
    Putchar('1'); 
    Putchar('2'); 
    Putchar('\b'); 
    Putchar('3'); 
    Puts("hello world\n\n\n\n");
    Putint(0);
    Putchar('\n');
    Putint(10);
    Putchar('\n');
    Putint(000000000);
    Putchar('\n');
    Putint(0x00ff1a0);
    Putchar('\n');
*/
    Puts("kernel start\n");
    init_all();
    void *p = get_kernel_memory(3);
    //Puts("next\n");
    //asm volatile("sti");	     // sti表示允许中断发生
    //ASSERT(1==2);
    while(1);
    return 0;
}