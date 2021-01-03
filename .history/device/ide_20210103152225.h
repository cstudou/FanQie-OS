#ifndef __DEVICE_IDE_H__
#define __DEVICE_IDE_H__
#include "../lib/kernel/print.h"
#include "../lib/kernel/stdint.h"
#include "../kernel/lock.h"
#include "../lib/kernel/bitmap.h"
#include "../kernel/stdio.h"
#include "timer.h"
struct Partition
{
    uint32_t     start_lba;          //起始扇区
    uint32_t     section_num;        //扇区数
    struct Disk* disk;               //分区所属硬盘
    struct ListPtr part_tag;         //队列中的标记
    char name[16];
    struct super_block* sb;         //超级块
    struct Bitmap block_bitmap;     //块位图
    struct Bitmap inode_bitmap;     //i结点位图
    struct List open_inodes;        //分区打开的inode队列
};

//硬盘
struct Disk
{
    char name[16];                  //名字
    struct Channel* channel;        //属于哪个ide通道
    uint8_t device_no;              //主硬盘还是从硬盘
    struct Partition prim_parts[4]; //主分区最多4个
    struct Partition logic_parts[8];//逻辑分区最多8个

};
struct Channel
{
    char        name[16];           //名字
    uint16_t    port_start;         //起始端口
    uint8_t     intr_num;           //中断号
    struct Lock lock;               //锁
    uint8_t     in_intr;            //等待硬盘的中断
    struct Semaphore disk_done;     //用于阻塞、唤醒驱动程序
    struct Disk devices[2];         //一个通道上可以连两个硬盘
};


extern struct Channel channels[2]; // 有两个 ide 通道
void ide_init();
void write_disk(struct Disk *disk, uint32_t lba, void *buf, uint32_t size);
#endif