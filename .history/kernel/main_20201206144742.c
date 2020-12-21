#include "../lib/kernel/print.h"
#include "../kernel/init.h"
#include "../kernel/assert.h"
#include "../kernel/thread.h"



void k_thread_a(void*); 
void k_thread_b(void*);
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
    //void *p = get_kernel_page(3);
    Puts("\n get_kernel_page start vaddr is "); 
    //Putint((uint32_t)p);
    Puts("\n");
    //Puts("next\n");
    //asm volatile("sti");	     // sti表示允许中断发生
    //ASSERT(1==2);
    //Putint(*(uint32_t*)(0xc009a000));
    thread_start("k_thread_a", 31, k_thread_a, (void *)("hello "));
    thread_start("k_thread_b", 8, k_thread_b, (void *)("world "));
    On_interrupt();
    
    while(1)
    {
        Puts("Main ");
    }
    return 0;
}


void k_thread_a(void *arg)
{
    char *pa = arg;
    while(1)
    {
        Puts(pa);
    }
}

void k_thread_b(void *arg)
{
    char *pa = arg;
    while(1)
    {
        Puts(pa);
    }
}
