#include "filesystem.h"
#include "dir.h"
struct Partition *partition;

/*
操作系统引导块--超级块--空闲块位图--inode位图--inode数组--根目录--空闲块区域
*/

void partition_format(struct Disk *disk, struct Partition *part)
{
    uint32_t boot_sector_sectors = 1;           //引导块（未使用）
    uint32_t super_block_sectors = 1;
    uint32_t inode_bitmap_sectors = 1;
    //最多4096个inode，inode数组所占扇区数
    uint32_t inode_table_sectors = (sizeof(struct Inode) * 4096 + 511) / 512;
    uint32_t used_sectors = boot_sector_sectors + super_block_sectors + inode_bitmap_sectors + \
                        inode_table_sectors;
    uint32_t free_sectors = part->section_num - used_sectors;

    //空闲位图占扇区数
    uint32_t block_bitmap_sectors;
    //1扇区512字节一字节8位可表示4096个扇区
    block_bitmap_sectors = (free_sectors + 4095) / 4096;
    uint32_t block_bitmap_len = free_sectors - block_bitmap_sectors;
    block_bitmap_sectors = (block_bitmap_len + 4095) / 4096;

    //定义超级块
    struct SuperBlock sb;
    sb.magic = 0x20210103;
    sb.sector_num = part->section_num;
    sb.inode_num = 4096;
    sb.lba_start = part->start_lba;
    //第0块是引导块第一块是超级块
    sb.block_bitmap_addr = part->start_lba + 2;
    sb.block_bitmap_size = block_bitmap_len;

    sb.inode_bitmap_addr = sb.block_bitmap_addr + sb.block_bitmap_size;
    sb.inode_bitmap_size = inode_bitmap_sectors;

    sb.inode_table_addr = sb.inode_bitmap_addr + sb.inode_bitmap_size;
    sb.inode_table_size = inode_table_sectors;

    sb.data_start_addr = sb.inode_table_addr + sb.inode_table_size;
    sb.root_inode = 0;
    sb.dir_entry_size = sizeof(struct DirEntry); 


    struct Disk *hd = part->disk;
    //将超级快写入
    write_disk(hd, part->start_lba+1, &sb, 1);
    uint32_t buf_size = (sb.block_bitmap_size >= \
        sb.inode_bitmap_size ? sb.block_bitmap_size : sb.inode_bitmap_size);
    buf_size = (buf_size >= sb.inode_table_size ? buf_size : sb.inode_table_size) * 512;
    uint8_t *buf = sys_malloc(buf_size);
    buf[0] = 0x1;
    uint32_t block_bytes = block_bitmap_len / 8;
    uint32_t block_bits = block_bitmap_len % 8;
    uint32_t last_size = 512 - (block_bytes % 512);
    //超出实际块数的部分直接置为已占用
    memset(&buf[block_bytes], 0xff, last_size);
    uint32_t index = 0;
    for(; index<=block_bits; index++)
    {
        buf[block_bytes] &= ~(1 << index);
    }
    //把块位图写入磁盘扇区
    write_disk(hd, sb.block_bitmap_addr, buf, sb.block_bitmap_size);
    //将inode位图写入
    memset(buf, 0, buf_size);
    buf[0] |= 0x1;
    write_disk(hd, sb.inode_bitmap_addr, buf, sb.inode_bitmap_size);

    //将inode数组写入
    memset(buf, 0, buf_size);
    struct Inode *i = (struct Inode *)buf;
    i->inode_size = sb.dir_entry_size * 2;  //.和..
    i->inode_num = 0;                       //根目录
    i->inode_sector[0] = sb.data_start_addr;
    write_disk(hd, sb.inode_table_addr, buf, sb.inode_table_size);

    //将根目录写入
    memset(buf, 0, buf_size);
    struct DirEntry *de = (struct DirEntry *)buf;
    memcpy(de->filename, ".", 1);
    de->inode_num = 0;
    de->type = DIRECTORY;
    de ++;
    memcpy(de->filename, "..", 2);
    de->inode_num = 0;
    de->type = DIRECTORY;
    write_disk(hd, sb.data_start_addr, buf, 1);
    sys_free(buf);

printk(" magic:0x%x\n part_lba_base:0x%x\n all_sectors:0x%x\n inode_cnt:0x%x\nblock_bitmap_lba:0x%x\n block_bitmap_sectors:0x%x\n inode_bitmap_lba:0x%x\n \
        inode_bitmap_sectors:0x%x\ninode_table_lba:0x%x\n  \
        inode_table_sectors:0x%x\ndata_start_lba:0x%x\n", sb.magic, sb.lba_start, sb.sector_num, sb.inode_num, \
    sb.block_bitmap_addr, sb.block_bitmap_size, sb.inode_bitmap_addr, sb.inode_bitmap_size, sb.inode_table_addr,    \
    sb.inode_table_size, sb.data_start_addr);


}

bool mount(struct ListPtr *list, int arg)
{
    char *name = (char *)arg;
    printk("%s\n", name);
    struct Partition temp;

    uint32_t len = (uint32_t)&temp.part_tag - (uint32_t)&temp;
    struct Partition *part = (struct Partition *)((uint32_t)list - len);
    if(!strcmp(part->name, name))
    {
        //找到
        partition = part;
        struct Disk *disk = part->disk;
        //超级块
        struct SuperBlock *sb = (struct SuperBlock *)sys_malloc(512);
        memset(sb, 0, 512);
        partition->sb = (struct SuperBlock *)sys_malloc(sizeof(struct SuperBlock));
        //读入超级块
        read_disk(disk, partition->start_lba+1, sb, 1);
        //复制到分区超级块
        memcpy(partition->sb, sb, sizeof(struct SuperBlock));
        //读位图
        partition->block_bitmap.bits = (uint8_t *)sys_malloc(sb->block_bitmap_size * 512);
        partition->block_bitmap.bitmap_len = sb->block_bitmap_size * 512;
        read_disk(disk, sb->block_bitmap_addr, partition->block_bitmap.bits, sb->block_bitmap_size);
    
        //读inode
        partition->inode_bitmap.bits = (uint8_t*)sys_malloc(sb->inode_bitmap_size * 512);
        partition->inode_bitmap.bitmap_len = sb->inode_bitmap_size * 512;
        read_disk(disk, sb->inode_bitmap_addr, partition->inode_bitmap.bits, sb->inode_bitmap_size);
    
        list_init(&partition->open_inodes);
        printk("mount %s done\n", partition->name);
        return true;
    }
    return false;
}


void filesystem_init()
{
    uint8_t channel_no = 0, dev_no, index = 0;
    struct SuperBlock *sb = (struct SuperBlock *)sys_malloc(512);
    printk("searching filesystem.....\n");
    uint8_t hd_cnt = *((uint8_t *)(0x475));
    
    uint8_t channel_cnt = (hd_cnt + hd_cnt - 1) / 2;
    while(channel_no < channel_cnt)
    {
        dev_no = 0;
        while(dev_no < 2)
        {
            if(dev_no == 0)
            {
                //主盘忽略
                dev_no++;
                continue;
            }
            
            struct Disk *hd = &channels[channel_no].devices[dev_no];
            struct Partition *part = hd->prim_parts; 
            while(index < 12)
            {
               // printk("hahah\n");
                if(index == 4)
                {
                    part = hd->logic_parts;
                }
                if(part->section_num != 0)
                {
                    //分区存在
                    memset(sb, 0, 512);
                    read_disk(hd, part->start_lba+1, sb, 1);
                    if(sb->magic == 0x20210103)
                    {
                        printk("%s has filesystem\n", part->name);
                    }
                    else
                    {
                        printk("cannot read");
                        partition_format(hd, part);
                    }
                }
                index++;
                part++; //下一分区
            }
            dev_no++;
        }
        channel_no++;
    }
    sys_free(sb);
    char temp[5] = "sdb1";
    
    list_traversal(&partition_list, mount, (int)temp);
}
