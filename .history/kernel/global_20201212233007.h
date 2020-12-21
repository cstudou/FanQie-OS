#ifndef __FANQIE_GLOBAL_H__
#define __FANQIE_GLOBAL_H__

#include "../lib/kernel/stdint.h"
#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3
#define TI_GDT 0
#define TI_LDT 1
#define SELECTOR_CODE  ((1 << 3) + (TI_GDT << 2) + RPL0)
#define SELECTOR_DATA  ((2 << 3) + (TI_GDT << 2) + RPL0)
#define SELECTOR_STACK ((2 << 3) + (TI_GDT << 2) + RPL0)
#define SELECTOR_VIDEO ((3 << 3) + (TI_GDT << 2) + RPL0)

#define IDT_DESCRIPTOR_P    1               //存在位
#define IDT_DESCRIPTOR_DPL0 0
#define IDT_DESCRIPTOR_DPL3 3
#define IDT_DESCRIPTOR_32_TYPE 0xe
#define IDT_DESCRIPTOR_16_TYPE 0x6
//中断门type位D110，其中D为1代表32位模式，type后面是固定值0，共12位
#define IDT_DESCRIPTOR_ATTRITUBE_DPL0 ((IDT_DESCRIPTOR_P << 7) + (IDT_DESCRIPTOR_DPL0 << 5) + IDT_DESCRIPTOR_32_TYPE)
#define IDT_DESCRIPTOR_ATTRITUBE_DPL3 ((IDT_DESCRIPTOR_P << 7) + (IDT_DESCRIPTOR_DPL3 << 5) + IDT_DESCRIPTOR_32_TYPE)


/*

| 31-24 段基址| G | D/B |  L ] AVL |  段界限19-16 | P | DPL(2) | S | TYPE(4) |   段基址23-16 |
|段基址15-0                         |         段界限15-0                              |

*/
// TSS属性
#define DESC_G_4k       1       //段界限单位大小，4kb
#define DESC_D_32       1       //指示有效地址（段内偏移地址）及操作数的大小
#define DESC_L          0       //是否是64位代码段
#define DESC_AVL        0       //cpu不用此位
#define DESC_P          1       //是否存在于内存
#define DESC_DPL_0      1       //特权级
#define DESC_DPL_1      1
#define DESC_DPL_2      2
#define DESC_DPL_3      3
#define DESC_S_CODE     1       //是否是系统段，凡是硬件运行需要用到的东西都可称 之为系统
#define DESC_S_DATA     1
#define DESC_S_SYS      0       //0表示系统段
#define DESC_TYPE_CODE  8       //代码段是可执行的、非依从的（一致性）、不可读的，已访问位 a 清 0
//转移后的特权级不与自己的 DPL 为主， 而是与转移前的低特权级一致，也就是听从、依从转移前的低特权级
#define DESC_TYPE_DATA  2       //数据段是不可执行的、向上扩展的、可写的，已访问位 a 清 0
#define DESC_TYPE_TSS   9       //B 位为 0 时，表示任务不繁忙

//user
//选择子
// | 索引 | TI(1) | PRL(2) |
#define SELECT_U_CODE   ((5 << 3) + (TI_GDT << 2) + RPL3)
#define SELECT_U_DATA   ((6 << 3) + (TI_GDT << 2) + RPL3)
#define SELECT_U_STACK  ((6 << 3) + (TI_GDT << 2) + RPL3)

#define GDT_ATTR_HIGH       ((DESC_G_4K << 7) + (DESC_D_32 << 6) + (DESC_L << 5) + (DESC_AVL << 4))
#define GDT_CODE_ATTR_HIGH  ((DESC_P << 7) + (DESC_DPL_3 << 5) + (DESC_S_CODE << 4) + DESC_TYPE_CODE)
#define GDT_DATA_ATTR_HIGH  ((DESC_P << 7) + (DESC_DPL_3 << 5) + (DESC_S_DATA << 4) + DESC_TYPE_DATA)

// TSS
#define TSS_DESC_D          0
#define TSS_ATTR_HIGH       ((DESC_G_4K << 7) + (DESC_D_32 << 6) + (DESC_L << 5) + (DESC_AVL << 4))
#define TSS_ATTR_LOW        ((DESC_P << 7) + (DESC_DPL_0 << 5) + (DESC_S_SYS << 4) + DESC_TYPE_TSS)
#define SELECTOR_TSS        ((4 << 3) + (TI_GDT << 2) + RPL0)
#endif