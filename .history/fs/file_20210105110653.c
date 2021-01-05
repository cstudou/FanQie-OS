#include "file.h"
#define MAX_FILE_NUM 32

struct File file_table[MAX_FILE_NUM];

int get_slot()
{
    //标准输入、输出、错误---0，1，2
    uint32_t index = 3;
    for(; index<MAX_FILE_NUM; ++index)
    {
        if(file_table[index].inode == (void*)0)
        {
            break;
        }
    }
    if(index == MAX_FILE_NUM)
    {
        return -1;
    }
    return index;
}

int install_pcb(uint32_t index)
{
    struct TaskStruct *task = running_thread();
    uint32_t in = 3;
    for(; in<8; ++in)
    {
        if(task->fd_table[in] == -1)
        {
            //找一个空位插入
            task->fd_table[in] = index;
            break;
        }
    }
    if(in == 8) 
    {
        return -1;
    }
    return in;
}

int inode_bitmap_alloc(struct Partition *part)
{
    int ret = bitmap_malloc(&part->inode_bitmap, 1);
    if(ret == -1)
    {
        return -1;
    }
    bitmap_set_musk(&part->inode_bitmap, ret, 1);
    return ret;
}

int block_bitmap_alloc(struct Partition *part)
{
    int ret = bitmap_malloc(&part->block_bitmap, 1);
    if(ret == -1)
    {
        return -1;
    }
    bitmap_set_musk(&part->block_bitmap, ret, 1);
    return ret + part->sb->data_start_addr;
}

//将内存中 bitmap 第 bit_idx 位所在的 512 字节同步到硬盘
void bitmap_sync(struct Partition *part, uint32_t index, uint8_t btmp)
{
    uint32_t offset_sector = index / 4096;
    uint32_t offset_size = offset_sector * 512;
    uint32_t lba_start;
    uint8_t *bitmap_offset;
    if(btmp == INODE_BITMAP)
    {
        lba_start = part->sb->inode_bitmap_addr + offset_sector;
        bitmap_offset = part->inode_bitmap.bits + offset_size;
    }
}