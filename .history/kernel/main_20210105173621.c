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
//#include "fork.h"
extern struct IoQueue keyboard_queue;

int test_var_a = 0, test_var_b = 0;
void u_prog_a(void);
void u_prog_b(void);
void init(void);

int main()
{
    init_all();
    create_user_process(u_prog_a, "user_prog_a");
    create_user_process(u_prog_b, "user_prog_b");  
    On_interrupt();
    
    
    CPuts(" main_pid:0x");
    CPutint(sys_getpid());
    CPutchar(' ');
    
    while(1) ;
    return 0;
}

void u_prog_a()
{
    test_var_a = getpid();
    //printf(" I am %s, my pid is %d%c", "fuck1", getpid(), '#');
    while(1);
}
void u_prog_b()
{
    test_var_b = getpid();
    //printf(" I am %s, my pid is %d%c", "fuck1", getpid(), '#');
    while(1);
}

void init()
{
    uint32_t temp = fork();
    if(temp)
    {
        printf(" I am father %s, my pid is %d%c", "fuck1", getpid(), '#');
    }
    while(1);
}