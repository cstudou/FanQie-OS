#ifndef __FS_DIR_H__
#define __FS_DIR_H__
struct Dir
{
    struct Inode *next;
    uint32_t dis_pos;       //记录在目录的偏移
    uint8_t dir_buf[512];   //目录缓存
};

//目录项
struct DirEntry
{
    char filename[32];
    uint32_t inode_num;     //文件的inode编号
    enum FileType type;     //文件类型
};

#endif