#ifndef __FS_INODE_H__
#define __FS_INODE_H__
#include "../lib/kernel/stdint.h"
#include "filesystem.h"
#include "../lib/kernel/list.h"
struct Inode
{
    uint32_t inode_num;
    uint32_t inode_size;        //文件大小
    uint32_t inode_open_num;    //打开次数
    uint8_t  write_flag;        //判断是否在写
    uint32_t inode_sector[13];  //0-11是直接块，12是一级间接块指针
    struct ListPtr inode_tag;
};


#endif