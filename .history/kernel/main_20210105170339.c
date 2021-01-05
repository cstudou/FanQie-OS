#include "../lib/kernel/print.h"
#include "../kernel/init.h"
#include "../kernel/assert.h"
#include "../kernel/thread.h"
#include "../device/console.h"
#include "process.h"
#include "syscall.h"
#include "stdio.h"
#include "../device/keyboard.h"
#include "syscall.h"
#include "fork.h"
extern struct IoQueue keyboard_queue;
void init();

void k_thread_a(void*);
void k_thread_b(void*);

int main(void) {
   Puts("I am kernel\n");
   init_all();
   On_interrupt();
   create_user_process(init, "u_prog_a");
  // create_user_process(u_prog_b, "u_prog_b");
  //uint32_t x = *((uint32_t *)(0xc009a000));
   // Putint(x);
  // thread_start("k_thread_a", 31, k_thread_a, "I am thread_a");
  // thread_start("k_thread_b", 31, k_thread_b, "I am thread_b");
   while(1);
   return 0;
}


void init(void *arg)
{
    uint32_t temp = fork();
    if(temp)
    {
       // printf("I am father, my pid is %d, child pid is %d\n", getpid(), temp);

    }
    else
    {
       // printf("I am chld, my pid is %d, ret pid is %d\n", getpid(), temp);

    }
    while(1);
    
}
