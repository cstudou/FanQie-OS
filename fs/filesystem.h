#ifndef __FS_FILESYSTEM_H__
#define __FS_FILESYSTEM_H__
#include "inode.h"
#include "../device/ide.h"
#include "superblock.h"

enum FileType
{
    UNKNOWN,
    NORMAL,         //普通文件
    DIRECTORY       //目录
};
extern struct Channel channels[2];
void filesystem_init();
#endif