#include "filesystem.h"

void partition_format(struct Disk *disk, struct Partition *part)
{
    uint32_t boot_sector_sectors = 1;
    uint32_t super_block_sectors = 1;
    uint32_t inode_bitmap_sectors = 1;
    uint32_t inode_table_sectors = (sizeof(struct Inode) * 4096 + 511) / 512;
    uint32_t used_sectors = boot_sector_sectors + super_block_sectors + inode_bitmap_sectors + \
                        inode_table_sectors;
    uint32_t free_sectors = part->section_num - used_sectors;


    uint32_t block_bitmap_sectors;
    block_bitmap_sectors = (free_sectors + 4095) / 4096;
    uint32_t block_bitmap_len = free_sectors - block_bitmap_sectors;
    block_bitmap_sectors = (block_bitmap_len + 4095) / 4096;

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
}