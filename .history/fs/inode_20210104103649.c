#include "inode.h"
struct InodePos
{
    bool two_sec;
    uint32_t sector_lba;        //inode所在扇区
    uint32_t offset;            //inode所在扇区的字节偏移
};

