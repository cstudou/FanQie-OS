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

#endif