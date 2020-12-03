#include "string.h"
#include "stdint.h"
#include "../../kernel/assert.h"

void memset(void *start, uint8_t data, uint32_t size)
{
    ASSERT(start != NULL);
    uint8_t *begin = (uint8_t*)(start);
    int i=0;
    for(; i<size; ++i)
    {
        *begin = data;
        begin ++;
    }
}

//把数据从souce拷贝到dist，拷贝size个
void memcpy(void *dist, const void *source, uint32_t size)
{
    ASSERT((dist != NULL) && (source != NULL));
    uint8_t *begin = (uint8_t *)(dist);
    const uint8_t *head = (const uint8_t *)(source);
    int i = 0;
    for(; i<size; ++i)
    {
        *begin++ = *head++;
    }
}