#include "memory.h"
#define PAGE_SIZE 4096  //页面大小4KB

//之前设置的栈顶指针是0x9f000，高位地址浪费掉，因为9e000-9f000要设置为pcb，pcb必须是自然页
//一字节位图可以映射4*8=32字节内存，一页位图可以映射32 * 1024 * 4 = 128MB
//预留四张位图，即512字节内存的映射空间，起始地址则为0x9a000

#define HEAP_STRT 0xc0100000    //0xc0000000-0xc00fffff为低1M空间


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
    uint32_t kernel_bitmap_len = kernel_free_pages / 8;     //字节
    uint32_t user_bitmap_len = user_free_pages / 8;
    uint32_t kernel_pool_start = used_memory;           //内核内存池起始地址
    uint32_t user_pool_start = kernel_pool_start + kernel_free_pages * PAGE_SIZE;

    kernel_pool.physical_addr_start = kernel_pool_start;
    kernel_pool.memory_pool_bitmap.bitmap_len = kernel_bitmap_len;
    kernel_pool.memory_pool_bitmap.bits = (void *)(0xc009a000);
    kernel_pool.memory_pool_size = kernel_free_pages * PAGE_SIZE;
    
    user_pool.memory_pool_size = user_free_pages * PAGE_SIZE;
    user_pool.physical_addr_start = user_pool_start; 
    user_pool.memory_pool_bitmap.bitmap_len = user_bitmap_len;
    user_pool.memory_pool_bitmap.bits = (void *)(0xc009a000 + kernel_bitmap_len);
    
    /**输出内存池信息**/    
    Puts("  kernel poll bitmap start:");
    Putint((int)(kernel_pool.memory_pool_bitmap.bits));     //位图起始地址
    Puts("  kernel poll physical addr start:");
    Putint(kernel_pool.physical_addr_start);
    Putchar('\n');

    Puts("  user poll bitmap start:");
    Putint((int)(user_pool.memory_pool_bitmap.bits));
    Puts("  user poll phtsical addr start:");
    Putint(user_pool.physical_addr_start);
    Puts("\n");

    bitmap_init(&kernel_pool.memory_pool_bitmap);
    bitmap_init(&user_pool.memory_pool_bitmap);

    //虚拟地址与物理地址一致,虚拟地址位图接在用户地址位图之后
    kernel_virtual_addr.virtual_addr_bitmap.bitmap_len = kernel_bitmap_len;
    kernel_virtual_addr.virtual_addr_bitmap.bits = (void *)(0xc009a000 + kernel_bitmap_len + user_bitmap_len);
    kernel_virtual_addr.virtual_addr_start = HEAP_STRT;
    bitmap_init(&kernel_virtual_addr.virtual_addr_bitmap);

    Puts("  memory poll init done\n");

}

void memory_init()
{
    Puts("memort init start\n");
    uint32_t memory_byte_total = *((uint32_t *)(0x700));
    memory_pool_init(memory_byte_total);
    Puts("memory init done\n");
}

//从内存池中申请size大小的内存
static void* get_virtual_addr(enum MemoryPoolFlag flag, uint32_t size)
{
    int virtual_start = 0, bit_index = -1;
    uint32_t count = 0;
    if(flag == KERNELPOOL)
    {
        bit_index = bitmap_malloc(&kernel_virtual_addr.virtual_addr_bitmap, size);
        if(bit_index <= 0)
        {
            return NULL;
        }
        while(count < bit_index)
        {
            bitmap_set_musk(&kernel_virtual_addr.virtual_addr_bitmap, bit_index+count, 1);
            count++;
        }
        virtual_start = kernel_virtual_addr.virtual_addr_start + bit_index * PAGE_SIZE;
    }
    else
    {
        /* code */
    }
    return (void *)virtual_start;
}

//找页表项指针
uint32_t *pte_ptr(uint32_t vaddr)
{
    //((vaddr & 0x003ff000) >> 12)返回中间10位
    //构造新的虚拟地址
    uint32_t *pte = (uint32_t*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + \
             ((vaddr & 0x003ff000) >> 12) * 4);
    return pte;
}

//找页目录项指针
uint32_t *pde_ptr(uint32_t vaddr)
{
    uint32_t *pde = (uint32_t *)(0xfffff000 + ((vaddr & 0xffc00000) >> 22));
    return pde;
}

//在物理内存池中分配一个物理页
static void *palloc(struct Pool *physical_pool)
{

}