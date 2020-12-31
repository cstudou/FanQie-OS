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

//用于malloc内存
struct MemoryBlock
{
    struct ListPtr memory_list;     //块链表结点
};
//内存快描述符
struct MemoryDesc
{
    uint32_t size;
    uint32_t arena_nums;
    struct List free_list;          //双向链表
};

//元信息，其用来提供内存块，页面会被拆分成memoryblock然后挂在链表上
//当 large 为 ture 时，cnt 表示的是本 arena 占用的页框数，
//否则 large 为 false 时，cnt 表示本 arena 中 还有多少空闲内存块可用
struct Arena
{
    struct MemoryDesc *desc;
    uint32_t cnt;
    bool large; 

};
struct MemoryDesc kernel_memory_desc[7]; //bytes 16,32,64,128,256,512,1024
void memory_desc_init(struct MemoryDesc *memory_desc);
void memory_init();
void *get_kernel_page(uint32_t size);
void *get_page();
void *sys_malloc(uint32_t size);
uint32_t addr_v2p(uint32_t vaddr);
#endif