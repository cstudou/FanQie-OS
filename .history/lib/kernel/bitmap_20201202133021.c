#include "bitmap.h"

void bitmap_init(struct Bitmap *bitmap)
{
    memset(bitmap, 0, bitmap->bitmap_len);
}
bool bitmap_mask_test(struct Bitmap* bitmap, uint32_t index);
int bitmap_malloc(struct Bitmap* bitmap, uint32_t size);    //申请空间返回下标
void bitmap_set_musk(struct Bitmap* bitmap, uint32_t index, int8_t value);