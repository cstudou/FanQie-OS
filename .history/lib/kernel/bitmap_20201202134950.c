#include "bitmap.h"

void bitmap_init(struct Bitmap *bitmap)
{
    memset(bitmap->bits, 0, bitmap->bitmap_len);
}

//判断index位是否是1
uint8_t bitmap_mask_test(struct Bitmap* bitmap, uint32_t index)
{
    uint32_t local = index / 8;     //位转字节
    uint32_t odd = index % 8;
    return (bitmap->bits[local]) & (1 << odd);
}

//在位图上连续申请size个位，成功返回下标失败返回-1
int bitmap_malloc(struct Bitmap* bitmap, uint32_t size) 
{
    uint32_t free_index = 0;
    //全部分配就是0xff
    while(bitmap->bits[free_index] == 0xff && free_index < bitmap->bitmap_len)
    {
        free_index++;
    }
    //判断是否分配完了
    ASSERT(free_index < bitmap->bitmap_len);
    if(free_index == bitmap->bitmap_len)
    {
        return -1;  //分配失败
    }
    //可以分配，起始index是freeindex
    //找到第一个不为1的位
    uint32_t bit_index = 0;
    while(bitmap->bits[free_index] & (uint8_t)(1 << bit_index))
    {
        bit_index++;
    }
    bit_index = free_index * 8 + bit_index;     //空闲位在位图内的坐标
    if(size == 1)
    {
        return bit_index;
    }
    uint32_t free_count = bitmap->bitmap_len*8 - bit_index + 1; //还有freecount可用


}
void bitmap_set_musk(struct Bitmap* bitmap, uint32_t index, int8_t value);