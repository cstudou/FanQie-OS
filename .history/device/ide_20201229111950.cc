#include "ide.h"
#define BIT_ALT_STAT_BSY    0x80 // 硬盘忙
#define BIT_ALT_STAT_DRDY   0x40 // 驱动器准备好
#define BIT_ALT_STAT_DRQ    0x8  // 数据传输准备好了

/* device 寄存器的一些关键位 */ 
#define BIT_DEV_MBS 0xa0 
#define BIT_DEV_LBA 0x40 
#define BIT_DEV_DEV 0x10

/* 一些硬盘操作的指令 */ 
#define CMD_IDENTIFY        0xec // identify指令
#define CMD_READ_SECTOR     0x20 // 读扇区指令 
#define CMD_WRITE_SECTOR    0x30 // 写扇区指令
  
uint8_t channel_cnt; // 按硬盘数计算的通道数 
struct Channel channels[2]; // 有两个 ide 通道


void ide_init()
{
    Puts("ide init start!\n");
    //在物理地址 0x475 处存储着主机上安装的硬盘的数量，它是由 BIOS 检测并写入的
    uint8_t hd_cnt = *((uint8_t *)(0x475));

    //一个ide通道有两个硬盘
    channel_cnt = (hd_cnt + hd_cnt - 1) / 2;
    struct Channel *channel;
    uint8_t index = 0;
    while(index++ < channel_cnt)
    {
        channel = &channels[index];
        sprintf(channel->name, "ide%d", index);
        if(index == 0)
        {
            channel->port_start = 0x1f0;    // ide0通道的起始端口号是0x1f0
            channel->intr_num = 0x20 + 14;  // 从片 8259a 上倒数第二的中断引脚
        }
        else if(index == 1)
        {
            channel->port_start = 0x170;    // ide1通道的起始端口号是0x170
            channel->intr_num = 0x20 + 15;  // 从 8259A 上的最后一个中断引脚
        
        }
        channel->in_intr = 0;               //未向硬盘写指令时不期待中断
        lock_init(&channel->lock);
        sem_init(&channel->disk_done, 0);
    }
    Puts("ide init done!\n");
}