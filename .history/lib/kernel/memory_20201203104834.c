#include "memory.h"
#define PAGE_SIZE 4096  //页面大小4KB

//之前设置的栈顶指针是0x9f000，高位地址浪费掉，因为9e000-9f000要设置为pcb，pcb必须是自然页
//一字节位图可以映射4*8=32字节内存，一页位图可以映射32 * 1024 * 4 = 128MB
//预留四张位图，即512字节内存的映射空间，起始地址则为0x9a000

#define HEAD_STRT 0xc0100000    //0xc0000000-0xc00fffff为低1M空间


struct VirtualAddr kernel_virtual_addr;
struct Pool kernel_pool, user_pool;     //内核内存池与用户内存池、


static void memory_pool_init(uint32_t memory_size)
{
    Puts("  memory pool init begin\n");
    uint32_t page_table_size = PAGE_SIZE * 256; //第0和768个页目录项，低769-1022共254个页目录项
    uint32_t used_memory = page_table_size + 0x100000;  //包括页表大小和1M
    uint32_t free_memory = memory_size - used_memory;   //剩余内存容量
    uint16_t all_free_pages = free_memory / PAGE_SIZE;

    uint32_t kernel_free_pages = all_free_pages / 2;
    uint32_t user_free_pages = all_free_pages - kernel_free_pages;

    //位图中一位表示一页
    uint32_t kernel_bitmap_len = kernel_free_pages / 8;
    uint32_t user_bitmap_len = user_free_pages / 8;
    uint32_t kernel_pool_start = used_memory;           //内核内存池起始地址
    uint32_t user_pool_start = kernel_pool_start + kernel_free_pages * PAGE_SIZE;

    kernel_pool.physical_addr_start = kernel_pool_start;
    kernel_pool.memory_pool_bitmap.bitmap_len = kernel_bitmap_len;
    kernel_pool.memory_pool_bitmap.bits = (void *)(0xc009a000);
    user_pool.physical_addr_start = user_pool_start; 
    user_pool.memory_pool_bitmap.bitmap_len = user_bitmap_len;
    user_pool.memory_pool_bitmap.bits = (void *)(0xc009a000 + kernel_bitmap_len);
    

    
}

