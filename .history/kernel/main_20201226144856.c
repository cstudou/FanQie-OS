#include "../lib/kernel/print.h"
#include "../kernel/init.h"
#include "../kernel/assert.h"
#include "../kernel/thread.h"
#include "../device/console.h"
#include "process.h"
#include "syscall.h"
#include "stdio.h"
#include "../device/keyboard.h"
extern struct IoQueue keyboard_queue;



void k_thread_a(void*); 
void k_thread_b(void*);
int main()
{


    init_all();
   
    //create_user_process(u_prog_a, "user_prog_a");
    //create_user_process(u_prog_b, "user_prog_b");  
    On_interrupt();
    
    
    CPuts(" main_pid:0x");
    CPutint(sys_getpid());
    CPutchar(' ');
    thread_start("k_thread_a", 10, k_thread_a, (void *)("I am thread a "));
    thread_start("k_thread_b", 10, k_thread_b, (void *)("I am thread b "));
    while(1) ;
    return 0;
}


void k_thread_a(void *arg)
{
    char* para = arg;
    void *addr = sys_malloc(33);
    CPuts(" I am thread_a, sys_malloc(33), addr is 0x"); 
    CPuts((int)addr);
    CPutchar('\n');
    while(1);
}

void k_thread_b(void *arg)
{
    
    char* para = arg;
    
    while(1);
}
