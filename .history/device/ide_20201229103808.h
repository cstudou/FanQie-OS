#ifndef __DEVICE_IDE_H__
#define __DEVICE_IDE_H__
#include "../lib/kernel/print.h"
#include "../lib/kernel/stdint.h"
#include "../kernel/lock.h"
struct Partition
{
    uint32_t     start_lba;          //起始扇区
    uint32_t     section_num;        //扇区数
    struct Disk* disk;               //分区所属硬盘
};


struct Disk
{
    char name[16];
    struct Channel* channel;
    uint8_t device_no;
    struct Partition prim_parts[4];
    struct Partition logic_parts[8];

};
struct Channel
{
    char        name[16];           //名字
    uint16_t    port_start;         //起始端口
    uint8_t     intr_num;           //中断号
    struct Lock lock;               //锁
    uint8_t     in_intr;            //等待硬盘的中断
    struct Semaphore disk_none;     //用于阻塞、唤醒驱动程序
    struct Disk devices[2];         //一个通道上可以连两个硬盘
};




#endif