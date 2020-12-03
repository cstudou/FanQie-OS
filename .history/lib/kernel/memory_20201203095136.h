#ifndef __FANQIE_MEMORY_H__
#define __FANQIE_MEMORY_H__

#include "stdint.h"
#include "../../kernel/assert.h"
#include "bitmap.h"

struct VirtualAddr
{
    struct Bitmap virtual_addr_bitmap;
    uint32_t virtual_addr_start;
};
#endif