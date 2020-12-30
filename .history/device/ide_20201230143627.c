#include "ide.h"
#define BIT_STAT_BSY    0x80 // 硬盘忙
#define BIT_STAT_DRDY   0x40 // 驱动器准备好
#define BIT_STAT_DRQ    0x8  // 数据传输准备好了

/* device 寄存器的一些关键位 */ 
#define BIT_DEV_MBS 0xa0 
#define BIT_DEV_LBA 0x40 
#define BIT_DEV_DEV 0x10

/* 一些硬盘操作的指令 */ 
#define CMD_IDENTIFY        0xec // identify指令,获取硬盘的身份信息
#define CMD_READ_SECTOR     0x20 // 读扇区指令 
#define CMD_WRITE_SECTOR    0x30 // 写扇区指令
  
uint8_t channel_cnt; // 按硬盘数计算的通道数 
struct Channel channels[2]; // 有两个 ide 通道


uint32_t lba_start = 0;             //总扩展分区的起始lba
uint8_t p_no = 0, l_no = 0;         //硬盘主分区和逻辑分区的下标
struct List partition_list;         //分区链表
struct PartitionTable
{
    uint8_t bootable;       // 是否可引导
    uint8_t start_head;     // 起始磁头号
    uint8_t start_sec;      // 起始扇区号
    uint8_t start_chs;      //起始柱面号
    uint8_t fs_type;        //分区类型
    uint8_t end_head;       //结束磁头号
    uint8_t end_sec;        //结束扇区号
    uint8_t end_chs;        //结束柱面号
    uint32_t start_lba;     //起始lba地址
    uint32_t sec_cnt;       //本分区的扇区数
}__attribute__ ((packed));

//引导扇区
struct BootSector
{
    uint8_t boot[446];              //引导代码
    struct PartitionTable partition_table[4]; //分区表中有4项，16*4
    uint16_t magic;                 //魔数0x55 0xaa

}__attribute__ ((packed));          //不允许编译器为对齐而在此结构中填充空隙



void select_disk(struct Disk *disk)
{
    uint8_t temp = BIT_DEV_LBA | BIT_DEV_MBS;
    if(disk->device_no == 1)
    {
        temp |= BIT_DEV_DEV;
    }
    outb(disk->channel->port_start+6, temp);
}
//向硬盘控制器写入起始扇区地址以及要读的扇区数
void select_sector(struct Disk* disk, uint32_t lba, uint32_t size)
{
    struct Channel *channel = disk->channel;
    outb(channel->port_start+2, size);
    outb(channel->port_start+3, lba);
    outb(channel->port_start+4, lba>>8);
    outb(channel->port_start+5, lba>>16);
    // lba 地址的第 24~27 位要存储在 device 寄存器的 0~3 位
    uint8_t temp = BIT_DEV_LBA | BIT_DEV_MBS;
    if(disk->device_no == 1)
    {
        temp |= BIT_DEV_DEV;
    }
    outb(channel->port_start+6, temp | lba >> 24);
}

//发命令
void cmd_out(struct Channel* channel, uint8_t cmd)
{
    channel->in_intr = 1;
    outb(channel->port_start+7, cmd);
}

//读size个扇区到buf
void read_sector(struct Disk *disk, void *buf, uint32_t size)
{
    size = size * 512;
    insw(disk->channel->port_start, buf, size/2);
}

//写buf中的size个扇区到硬盘
void write_sector(struct Disk *disk, void *buf, uint32_t size)
{
    size = size * 512;
    outsw(disk->channel->port_start, buf, size/2);
}

uint8_t busy_wait(struct Disk *disk)
{
    uint32_t temp_time = 30*1000;
    while(temp_time > 0)
    {
        if (!(readb(disk->channel->port_start+7) & BIT_STAT_BSY)) 
        { 
            return (readb(disk->channel->port_start+7) & BIT_STAT_DRQ);
        } 
        else 
        {
            ticks_sleep(10);
        }
        temp_time -= 10;
    }
    return 0;
}


//从硬盘读size个扇区到buf
void read_disk(struct Disk *disk, uint32_t lba, void *buf, uint32_t size)
{
    lock_acquire(&disk->channel->lock);
    select_disk(disk);                      //选择操作的硬盘
    uint32_t sec_do, sec_done = 0;
    while(sec_done < size)
    {
        //每次操作的扇区数
        if(sec_done + 256 < size)   sec_do = 256;
        else                        sec_do = size - sec_done;

        select_sector(disk, lba, size);         //写入待读入的扇区数和起始扇区号
        cmd_out(disk->channel, CMD_READ_SECTOR);    //准备读数据
        
        sem_wait(&disk->channel->disk_done);        //阻塞
        if(!busy_wait(disk))                        //失败
        {
            Puts(disk->name);
            Puts(" read sector ");
            Putint(lba);
            Puts(" failed\n");
            return ;
        }
        read_sector(disk, (void *)((uint32_t)buf+sec_done*512), sec_do);
        sec_done += sec_do;

    }
    lock_release(&disk->channel->lock);    
}

//写size个扇区到硬盘
void write_disk(struct Disk *disk, uint32_t lba, void *buf, uint32_t size)
{
    lock_acquire(&disk->channel->lock);
    select_disk(disk);                      //选择操作的硬盘
    uint32_t sec_do, sec_done = 0;
    //8位寄存器最多读255个
    while(sec_done < size)
    {
        //每次操作的扇区数
        if(sec_done + 256 < size)   sec_do = 256;
        else                        sec_do = size - sec_done;

        select_sector(disk, lba, size);         //写入待读入的扇区数和起始扇区号
        cmd_out(disk->channel, CMD_WRITE_SECTOR);    //准备读数据
        
        
        if(!busy_wait(disk))                        //失败
        {
            Puts(disk->name);
            Puts(" read sector ");
            Putint(lba);
            Puts(" failed\n");
            return ;
        }
        write_sector(disk, (void *)((uint32_t)buf+sec_done*512), sec_do);
        sem_wait(&disk->channel->disk_done);        //阻塞，参数更新需要互斥
        sec_done += sec_do;

    }
    lock_release(&disk->channel->lock);
}

void intr_disk_handler(void* intr_no)
{
    uint8_t no = *(uint8_t *)intr_no - 0x2e;
    struct Channel *channel = &channels[no];
    if(channel->in_intr)
    {
        channel->in_intr = 0;
        sem_post(&channel->disk_done);       //解开阻塞，硬盘完成操作后会发中断信号
        readb(channel->port_start + 7);
    }
}


void swap_bytes(const char *x, char *buf, uint32_t len)
{
    uint8_t index;
    for(index = 0; index < len; index+=2)
    {
        buf[index + 1] = *x;
        x++;
        buf[index]   = *x;
        x++;
    }
    buf[index] = '\0';
}


//获取硬盘信息
void identify_disk(struct Disk *disk)
{
    char buf[512];              //发送 identify 命令后返回的硬盘参数
    select_disk(disk);
    cmd_out(disk->channel, CMD_IDENTIFY);
    sem_wait(&disk->channel->disk_done);
    //-------
    read_sector(disk, buf, 1);
    char temp[64];
    uint8_t sn_start = 10 * 2, sn_len = 20, md_start = 27 * 2, md_len = 40; 
    swap_bytes(&buf[sn_start], temp, sn_len);
    //printk(" disk %s info:\n SN: %s\n", hd->name, buf);
    Puts(" disk "); Puts(disk->name); Puts(" info:\n SN: "); Puts(temp); Putchar('\n');
    memset(temp, 0, sizeof(temp));
    swap_bytes(&buf[md_start], temp, md_len);
    Puts(" Module: "); Puts(buf); Putchar('\n');
    //printk(" MODULE: %s\n", buf);
    uint32_t sectors = *(uint32_t*)&buf[60 * 2];
    //printk(" SECTORS: %d\n", sectors);
    Puts(" Sectors: "); Putint(sectors); Putchar('\n');
    Puts(" Capacity: "); Putint(sectors * 512 / 1024 / 1024); Puts("MB/n");
    //printk(" CAPACITY: %dMB\n", sectors * 512 / 1024 / 1024);
}


void partitioan_scan(struct Disk *disk, uint32_t lba)
{
    struct BootSector* boot_sector = sys_malloc(sizeof(struct Partition));
    read_disk(disk, lba, boot_sector, 1);
    uint8_t index = 0;
    struct PartitionTable *table = boot_sector->partition_table;
    //遍历四个分区表
    while(index++ < 4)
    {
        if(table->fs_type == 0x5)
        {
            //扩展分区
            if(lba_start != 0)
            {
                partitioan_scan(disk, table->start_lba+lba_start);
            }
            else
            {
                //第一次读取引导快
                lba_start = table->start_lba;
                partitioan_scan(disk, table->start_lba);   
            }
        }
        else if (table->fs_type != 0)
        {
            //有效的分区类型
            if(lba == 0)
            {
                disk->prim_parts[p_no].start_lba = lba_start + table->start_lba;
                disk->prim_parts[p_no].section_num = table->sec_cnt;
                disk->prim_parts[p_no].disk = disk;
                list_push_back(&partition_list, &disk->prim_parts[p_no].part_tag);
                sprintf(disk->prim_parts[p_no].name, "%s%d", disk->name, p_no+1);
                p_no++;
            }
            else
            {
                disk->prim_parts[l_no].start_lba = lba_start + table->start_lba;
                disk->prim_parts[l_no].section_num = table->sec_cnt;
                disk->prim_parts[l_no].disk = disk;
                list_push_back(&partition_list, &disk->prim_parts[l_no].part_tag);
                sprintf(disk->prim_parts[l_no].name, "%s%d", disk->name, l_no+5);
                l_no++;
                if(l_no >= 8)    return ;
            }
            
        }
        //p++;
    }
    sys_free(boot_sector);
}

bool info(struct ListPtr *list, int arg )
{
    struct Partition pa;
    uint32_t len =  (uint32_t)&pa.part_tag - (uint32_t)&pa;
    struct Partition *part = (struct Partition *)((uint32_t)list - len);
    Puts("      "); Puts(part->name); Puts(" start_lba:0x");
    Putint(part->start_lba); Puts(", sec_cnt:0x"); Putint(part->section_num);
    Puts("\n");
    return false;
}




void ide_init()
{
    Puts("ide init start!\n");
    //在物理地址 0x475 处存储着主机上安装的硬盘的数量，它是由 BIOS 检测并写入的
    uint8_t hd_cnt = *((uint8_t *)(0x475));
    list_init(&partition_list);
    //一个ide通道有两个硬盘
    channel_cnt = (hd_cnt + hd_cnt - 1) / 2;
    struct Channel *channel;
    uint8_t index = 0, dev_no = 0;
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
        register_intr_handler(channel->intr_num, intr_disk_handler);

        while(dev_no < 2)
        {
            struct Disk *hd = &channels->devices[dev_no];
            hd->channel = channel;
            hd->device_no = dev_no;
            sprintf(hd->name, "sd%c", 'a' + index * 2 + dev_no);
            identify_disk(hd);
            if(dev_no)
            {
                partitioan_scan(hd, 0);
            }
            p_no = 0, l_no = 0;
        }
        dev_no = 0;
    }
    Puts("ide init done!\n");
}
