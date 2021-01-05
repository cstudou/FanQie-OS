#include "inode.h"
struct InodePos
{
    bool two_sec;
    uint32_t sector_lba;        //inode所在扇区
    uint32_t offset;            //inode所在扇区的字节偏移
};

//inode所在扇区以及偏移
void inode_locate(struct Partition *part, uint32_t inode, struct InodePos *pos)
{
    uint32_t inode_table_addr = part->sb->inode_table_addr;
    uint32_t inode_size = sizeof(struct Inode);
    uint32_t offset_size = inode * inode_size;
    uint32_t offset_sector = offset_size / 512;
    uint32_t offset_size_in_sector = offset_size % 512;
    uint32_t temp = 512 - offset_size_in_sector;
    if(temp < inode_size)
    {
        pos->two_sec = true;        //跨越两个扇区
    }
    else
    {
        pos->two_sec = false;
    }
    pos->sector_lba = inode_table_addr + offset_sector;
    pos->offset = offset_size_in_sector;
}

//将inode写到分区
void inode_sync(struct Partition *part, struct Inode *inode, void *buf)
{
    struct InodePos pos;
    inode_locate(part, inode->inode_num, &pos);
    
    struct Inode temp_inode;
    memcpy(&temp_inode, inode, sizeof(struct Inode));
    temp_inode.inode_open_num = 0;
    temp_inode.write_flag = false;
    temp_inode.inode_tag.next = temp_inode.inode_tag.prev = (void *)0;

    if(pos->two_sec)
    {

    }


}