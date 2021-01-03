#include "filesystem.h"

void partition_format(struct Disk *disk, struct Partition *part)
{
    uint32_t boot_sector_sectors = 1;
    uint32_t super_block_sectors = 1;
    uint32_t inode_bitmap_sectors = 1;
    //最多4096个inode
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

    sb.inode_bitmap_addr = part->start_lba + sb.block_bitmap_size;
    sb.inode_bitmap_size = inode_bitmap_sectors;

    sb.inode_table_addr = part->start_lba + sb.inode_bitmap_size;
    sb.inode_table_size = inode_table_sectors;

    sb.data_start_addr = part->start_lba + sb.inode_table_size;
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
    write_disk(hd, sb.block_bitmap_addr, buf, sb.block_bitmap_size);



}