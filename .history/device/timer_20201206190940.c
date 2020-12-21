#include "timer.h"

#define IRQ0_FREQUENCY 100
#define INPUT_FREQUENCY 1193180
#define COUNTER0_VALUE INPUT_FREQUENCY / IRQ0_FREQUENCY //1193180/计数器 0 的初始计数值=中断信号的频率
#define COUNTRE0_PORT 0x40
#define COUNTER0_NO 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_port 0x43
uint32_t kernel_clock;
static void set_frequency(uint8_t counter_port, uint8_t counter_no, uint8_t rwl, uint8_t counter_mode, uint16_t counter_value)
{
    //向0x43写控制字，指定使用的计数器设置控制模式
    //计数器index（2）--读写方式（2）--工作方式（3）--数值格式（1）
    outb(PIT_CONTROL_port, (uint8_t)((COUNTER0_NO<<6)+(rwl<<4)+(COUNTER_MODE<<1)));
    //写入count_value低8位(计数初值)，方式2为0x40端口
    outb(counter_port, (uint8_t)counter_value);
    //写入count_value高8位
    outb(counter_port, (uint8_t)(counter_value>>8));
}

//时钟信号处理
static void intr_timer_handler(void * args)
{
    struct TaskStruct *thread = running_thread();
    ASSERT(thread->stack_margin == 0x19977991);
    kernel_clock++;   //时间+1
    thread->total_time ++;
    if(thread->clock == 0)  //线程时间片用完
    {
        Puts("\n\nthis is schedule\n\n");
        schedule();
    }
    else
    {
        thread->clock --;
        Putint(thread->clock);
        Puts("\n");
    }
    

}

void timer_init()
{
    Puts("timer init start\n");
    set_frequency(COUNTRE0_PORT,COUNTER0_NO,READ_WRITE_LATCH,\
            COUNTER_MODE, COUNTER0_VALUE);
    register_intr_handler(0x20, intr_timer_handler);
    Puts("timer init done\n");    
}