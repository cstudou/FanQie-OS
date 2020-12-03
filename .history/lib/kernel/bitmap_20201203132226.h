#ifndef __FANQIE_BITMAP_H__
#define __FANQIE_BITMAP_H__
#include "stdint.h"
#include "print.h"
#include "assert.h"
#include "../../kernel/interrupt.h"
#include "string.h"

struct Bitmap
{
    uint32_t bitmap_len;
    uint8_t* bits;
};

void bitmap_init(struct Bitmap *bitmap);
uint8_t bitmap_mask_test(struct Bitmap* bitmap, uint32_t index);
int bitmap_malloc(struct Bitmap* bitmap, uint32_t size);    //申请空间返回下标
void bitmap_set_musk(struct Bitmap* bitmap, uint32_t index, int8_t value);
#endif