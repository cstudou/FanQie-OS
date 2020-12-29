#ifndef __FANQIE_MEMORY_H__
#define __FANQIE_MEMORY_H__

#include "stdint.h"
#include "../../kernel/assert.h"
#include "bitmap.h"
#include "print.h"
#include "../../kernel/lock.h"
#define PG_P_1 1
#define PG_P_0 0
#define PG_RW_R 0
#define PG_RW_W 2
#define PG_US_S 0   //US=0，表示只允许特权级别为0、1、2的程序访问此页内存
#define PG_US_U 4
#define NULL ((void*)0)

// 页表项或页目录项存在属性位 // 页表项或页目录项存在属性位 // R/W 属性位值，读/执行
// R/W 属性位值，读/写/执行 // U/S 属性位值，系统级
// U/S 属性位值，用户级
struct VirtualAddr
{
    struct Bitmap virtual_addr_bitmap;
    uint32_t virtual_addr_start;            //虚拟地址的起始值
};


//内存池
struct Pool
{
    struct Bitmap memory_pool_bitmap;   //位图
    uint32_t physical_addr_start;       //物理地址起始
    uint32_t memory_pool_size;          //内存容量
    struct Lock lock;
};


enum MemoryPoolFlag
{
    KERNELPOOL = 1,
    USERPOOL = 2        //用户内存池
};

void memory_init();
void *get_kernel_page(uint32_t size);
void *get_page();
uint32_t addr_v2p(uint32_t vaddr);
#endif