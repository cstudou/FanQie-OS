#ifndef __FANQIE_BITMAP_H__
#define __FANQIE_BITMAP_H__
#include "stdint.h"
#include "print.h"
#include "assert.h"
#include "../../kernel/interrupt.h"
#inlcude "string.h"
#define BITNODE 1
struct Bitmap
{
    uint32_t bitmap_len;
    uint8_t* bits;
};

#endif