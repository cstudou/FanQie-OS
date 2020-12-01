#include "time.h"

#define IRQ0_FREQUENCY 100
#define INPUT_FREQUENCY 1193180
#define COUNTER0_value INPUT_FREQUENCY / IRQ0_FREQUENCY
#define COUNTRE0_PORT 0x40
#define COUNTER0_NO 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_port 0x43

static void set_frequency(uint8_t counter_port, uint8_t counter_no, uint8_t rwl, uint8_t counter_mode, uint16_t counter_value)
{
    //向0x43写控制字，指定使用的计数器设置控制模式
    outb(PIT_CONTROL_port, (COUNTER0_NO<<6)+(rwl<<4)+(COUNTER_MODE<<1);
    outb(counter_port, (uint8_t)counter_value);
    outb(counter_port, (uint8_t)counter_value>>8)
}