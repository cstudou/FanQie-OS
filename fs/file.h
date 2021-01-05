#ifndef __FS_FILE_H__
#define __FA_FILE_H__
#include "inode.h"
#include "../lib/kernel/stdint.h"
struct File
{
    uint32_t fd_offset;     //文件偏移
    uint32_t fd_flag;       //文件操作标识
    struct Inode *inode;
};
enum BitmapStatus
{
    INODE_BITMAP, BLOCK_BITMAP
};
#endif
