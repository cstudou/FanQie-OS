#include "filesystem.h"

void partition_format(struct Disk *disk, struct Partition *part)
{
    uint32_t boot_sector_sectors = 1;
    uint32_t super_block_sectors = 1;
    uint32_t inode_bitmap_sectors = 1;
    uint32_t inode_table_sectors = (sizeof(struct Inode) * 4096 + 511) / 512;
}