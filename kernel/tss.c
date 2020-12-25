#include "tss.h"

//初始化并定义tss
//tss段描述符
//用户代码段描述符
//用户数据段描述符
static struct Tss tss;
void update_tss_esp(struct TaskStruct *task)
{
    //调进程时会保存0级栈，中断来的时候栈就是pcb顶部
    tss.esp0 = (uint32_t *)((uint32_t)task + 4096);
}


static struct GdtDesc make_gdt_desc(uint32_t *desc_addr, uint32_t limit, uint8_t attr_low, uint8_t attr_high)
{
    uint32_t base = (uint32_t)desc_addr;
    struct GdtDesc desc;
    desc.limit_low = limit & 0x0000ffff; //低16位
    desc.limit_high_attr_high = ((limit & 0x000f0000) >> 16) + (uint8_t)attr_high;
    desc.attr_low = attr_low;
    desc.base_low = base & 0x0000ffff;
    desc.base_mid = (base & 0x00ff0000) >> 16;
    desc.base_high = base >> 24;
    return desc;
}


void tss_init()
{
    Puts("tss init start!\n");
    uint32_t tss_size = sizeof(struct Tss);
    memset(&tss, 0, tss_size);
    tss.ss0 = SELECTOR_STACK;
    tss.io_base = tss_size;         //当 IO 位图的偏移地址 大于等于 TSS 大小减 1 时，就表示没有 IO 位图。
    //tss
    *((struct GdtDesc *)0xc0000520) = make_gdt_desc((uint32_t *)&tss, tss_size-1, TSS_ATTR_LOW, TSS_ATTR_HIGH);
    //代码段
    *((struct GdtDesc *)0xc0000528) = make_gdt_desc((uint32_t *)0, 0xfffff, GDT_CODE_ATTR_LOW, GDT_ATTR_HIGH);
    //数据段
    *((struct GdtDesc *)0xc0000530) = make_gdt_desc((uint32_t *)0, 0xfffff, GDT_DATA_ATTR_LOW, GDT_ATTR_HIGH);

    // 重新加载gdt
    // |32位基址|16位界限|
    // 一共7个描述符
    //先强转！！！精度丢失
    uint64_t new_gdt = (uint64_t)(((uint64_t)0xc0000500 << 16) + (uint16_t)(7 * 8 - 1));

    asm volatile("lgdt %0" ::"m"(new_gdt));
    asm volatile("ltr %w0"::"r"(SELECTOR_TSS));
    Puts("tss init done!\n");
}