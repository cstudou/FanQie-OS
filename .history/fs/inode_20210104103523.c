#include "inode.h"
struct InodePos
{
    bool two_sec;
    uint32_t sector_lba;        //inode所在扇区
    uint32_t offset;            //inode所在扇区的字节偏移
};

void inode_locate(struct Partition *part, uint32_t inode, struct InodePos *pos)
{
    
}