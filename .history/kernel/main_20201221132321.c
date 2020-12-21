#include "../lib/kernel/print.h"
#include "../kernel/init.h"
#include "../kernel/assert.h"
#include "../kernel/thread.h"
#include "../device/console.h"
#include "process.h"
#include "../device/keyboard.h"
extern struct IoQueue keyboard_queue;

int test_var_a = 0, test_var_b = 0;
void u_prog_a();
void u_prog_b();
void k_thread_a(void*); 
void k_thread_b(void*);
int main()
{


    init_all();
   
    thread_start("k_thread_a", 10, k_thread_a, (void *)("argA "));
    thread_start("k_thread_b", 10, k_thread_b, (void *)("argB "));
    create_user_process(u_prog_a, "user_prog_a");
    create_user_process(u_prog_b, "user_prog_b");
    On_interrupt();
  
    while(1) ;
    return 0;
}


void k_thread_a(void *arg)
{
    char *pa = arg;
    while(1)
    {
        //enum InterruptStatus status = Off_interrupt();
        //if(!ioqueue_empty(&keyboard_queue))
        //{
        //    CPuts(pa);
        //    char str = ioqueue_getchar(&keyboard_queue);
        //    CPutchar(str);
        //}

        //set_InterruptStatus(status);
        Puts("V_A:0x");
        Putint(test_var_a);
    }
}

void k_thread_b(void *arg)
{
    
    char *pa = arg;
    CPuts(pa);
    while(1)
    {
        //enum InterruptStatus status = Off_interrupt();
        //if(!ioqueue_empty(&keyboard_queue))
        //{
        //    CPuts(pa);
        //    char str = ioqueue_getchar(&keyboard_queue);
        //    CPutchar(str);
            //CPutint(keyboard_queue.tail);
        //}
        
        //Off_interrupt();
        //Puts(pa);
        //On_interrupt();
        //set_InterruptStatus(status);
        Puts(" V_B:0x");
        Putint(test_var_b);
    }
}
void u_prog_a()
{
    while(1) test_var_a++;
}
void u_prog_b()
{
    while(1) test_var_b++;
}
