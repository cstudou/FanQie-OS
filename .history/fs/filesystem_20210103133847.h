#ifndef __FS_FILESYSTEM_H__
#define __FS_FILESYSTEM_H__
#include "inode.h"
#include "../device/ide.h"
#include "superblock.h"
#include "dir.h"
enum FileType
{
    UNKNOWN,
    NORMAL,         //普通文件
    DIRECTORY       //目录
};
#endif