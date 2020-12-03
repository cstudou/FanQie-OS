#ifndef __FANQIE_BITMAP_H__
#define __FANQIE_BITMAP_H__
#include "stdint.h"

#define BITNODE 1
struct Bitmap
{
    uint32_t bitmap_len;
    unt8_t* bits;
};

#endif