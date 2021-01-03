#ifndef __FS_SUPERBLOCK_H__
#define __FS_SUPERBLOCK_H__
#include "../lib/kernel/stdint.h"
struct SuperBlock
{
    uint32_t magic;                     //魔数，表示文件系统类型
    uint32_t sector_num;                //扇区数
    uint32_t inode_num;                 //inode数
    uint32_t lba_start;                 //起始扇区地址
    uint32_t block_bitmap_addr;         //空闲块位图地址
    uint32_t block_bitmap_size;         //空闲块位图占多少扇区
    uint32_t inode_bitmap_addr;         //inode位图地址
    uint32_t inode_bitmap_size;         //inode位图大小
    uint32_t inode_table_addr;          //inode表地址
    uint32_t inode_table_size;          //inode表大小
    uint32_t data_start_addr;           //数据区起始扇区
    uint32_t root_inode;                //根目录inode
    uint32_t dir_entry_size;            //目录项大小
    uint8_t pad[460];                   //填充到512字节
}__attribute__(packed);
#endif