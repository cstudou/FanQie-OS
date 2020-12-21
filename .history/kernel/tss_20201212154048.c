#include "tss.h"


static struct Tss tss;
void update_tss_esp(struct TaskStruct *task)
{
    tss.esp0 = (uint32_t *)((uint32_t)task + 4096);
}


static struct GdtDesc make_gdt_desc(uint32_t *desc_addr, uint32_t limit, uint8_t attr_low, uint8_t attr_high)
{
    uint32_t base = (uint32_t)desc_addr;
    struct GdtDesc desc;
    desc.limit_low = limit & 0x0000ffff; //低16位
    desc.limit_high_attr_high = ((limit & 0x000f0000) >> 16) + (uint8_t)attr_high;
    desc.base_low = desc_addr & 0x0000ffff;
    desc.base_mid = (desc_addr & 0x00ff0000) >> 16;
    
}