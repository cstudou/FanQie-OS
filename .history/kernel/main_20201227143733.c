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

/*

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
    CPutint((int)addr);
    CPutchar('\n');
    while(1);
}

void k_thread_b(void *arg)
{
    
    char* para = arg;
    void *addr = sys_malloc(63);
    CPuts(" I am thread_a, sys_malloc(33), addr is 0x"); 
    CPutint((int)addr);
    CPutchar('\n');
    while(1);
}

*/


void k_thread_a(void*);
void k_thread_b(void*);

int main(void) {
   Puts("I am kernel\n");
   init_all();
   On_interrupt();
  // create_user_process(u_prog_a, "u_prog_a");
  // create_user_process(u_prog_b, "u_prog_b");
   thread_start("k_thread_a", 31, k_thread_a, "I am thread_a");
   thread_start("k_thread_b", 31, k_thread_b, "I am thread_b");
   while(1);
   return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {     
   char* para = arg;
    void* addr1;
    void* addr2;
    void* addr3;
    void* addr4;
    void* addr5;
    void* addr6;
    void* addr7;
    CPuts(" thread_a start\n"); 
    int max = 1000;
    while (max-- > 0) {
        int size = 128;
        addr1 = sys_malloc(size);
        size *= 2;
        addr2 = sys_malloc(size);
        size *= 2;
        addr3 = sys_malloc(size);
        sys_free(addr1);
        addr4 = sys_malloc(size);
        size *= 2; size *= 2; size *= 2; size *= 2; size *= 2; size *= 2; size *= 2;
        addr5 = sys_malloc(size);
        addr6 = sys_malloc(size);
        sys_free(addr5);
        size *= 2;
        addr7 = sys_malloc(size);
        sys_free(addr6);
        sys_free(addr7);
        sys_free(addr2);
        sys_free(addr3);
        sys_free(addr4);
    }
    CPuts(" thread_a end\n"); while(1);
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {     
   char* para = arg;
    void* addr1;
    void* addr2;
    void* addr3;
    void* addr4;
    void* addr5;
    void* addr6;
    void* addr7;
    void* addr8;
    void* addr9;
    CPuts(" thread_b start\n"); 
    int max = 1000;
    while (max-- > 0) {
        CPutint(max);
        CPutchar('\n');
        int size = 9;
        addr1 = sys_malloc(size);
        size *= 2;
        addr2 = sys_malloc(size);
        size *= 2;
        sys_free(addr2);
        addr3 = sys_malloc(size);
        sys_free(addr1);
        addr4 = sys_malloc(size);
        addr5 = sys_malloc(size);
        addr6 = sys_malloc(size);
        sys_free(addr5);
        size *= 2;
        addr7 = sys_malloc(size);
        sys_free(addr6);
        sys_free(addr7);
        sys_free(addr3);
        sys_free(addr4);

        size *= 2; size *= 2; size *= 2;
        addr1 = sys_malloc(size);
        addr2 = sys_malloc(size);
        addr3 = sys_malloc(size);
        addr4 = sys_malloc(size);
        addr5 = sys_malloc(size);
        addr6 = sys_malloc(size);
        addr7 = sys_malloc(size);
        addr8 = sys_malloc(size);
        addr9 = sys_malloc(size);
        sys_free(addr1);
        sys_free(addr2);
        sys_free(addr3);
        sys_free(addr4);
        sys_free(addr5);
        sys_free(addr6);
        sys_free(addr7);
        sys_free(addr8);
        sys_free(addr9);
    }
    CPuts(" thread_b end\n"); while(1);
}
