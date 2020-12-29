#include "memory.h"
#include "../../kernel/thread.h"

#define PAGE_SIZE 4096  //页面大小4KB

//之前设置的栈顶指针是0x9f000，高位地址浪费掉，因为9e000-9f000要设置为pcb，pcb必须是自然页
//一字节位图可以映射4*8=32字节内存，一页位图可以映射32 * 1024 * 4 = 128MB
//预留四张位图，即512字节内存的映射空间，起始地址则为0x9a000


//1.初始化用户内存池、内核内存池、虚拟内存池
//2.申请页面先申请虚拟内存池
//3.用虚拟内存池申请的东西对应物理内存池，并添加页表映射
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

    //内核页与用户页对半分
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


//从内存池中申请size大小的内存
static void* get_virtual_addr(enum MemoryPoolFlag flag, uint32_t size)
{
    int virtual_start = 0, bit_index = -1;
    uint32_t count = 0;
    if(flag == KERNELPOOL)
    {
        bit_index = bitmap_malloc(&kernel_virtual_addr.virtual_addr_bitmap, size);
        if(bit_index < 0)
        {
            return NULL;
        }
        while(count < size)
        {
            //分配成功将bitmap置位
            bitmap_set_musk(&kernel_virtual_addr.virtual_addr_bitmap, bit_index+count, 1);
            count++;
        }
        virtual_start = kernel_virtual_addr.virtual_addr_start + bit_index * PAGE_SIZE;
    }
    else
    {
        //用户内存池
        struct TaskStruct *task = running_thread();
        bit_index = bitmap_malloc(&task->user_addr.virtual_addr_bitmap, size);  //从用户虚拟内存池中分配
        if(bit_index < 0)
        {
            return NULL;
        }
        while(count < size)
        {
            //分配成功将bitmap置位
            bitmap_set_musk(&task->user_addr.virtual_addr_bitmap, bit_index+count, 1);
            count++;
        }
        virtual_start = task->user_addr.virtual_addr_start + bit_index * PAGE_SIZE;
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
    uint32_t *pde = (uint32_t *)(0xfffff000 + ((vaddr & 0xffc00000) >> 22) * 4); //一定要乘以4！！！！！
    return pde;
}

//在物理内存池中分配一个物理页
static void *palloc(struct Pool *physical_pool)
{
    int bit_index = bitmap_malloc(&physical_pool->memory_pool_bitmap, 1);
    if(bit_index == -1)
    {
        return NULL;
    }
    bitmap_set_musk(&physical_pool->memory_pool_bitmap, bit_index, 1);
    uint32_t page = (physical_pool->physical_addr_start + bit_index * PAGE_SIZE);
    return (void *)page;
}

//页表中添加虚拟地址vaddr到物理地址paddr的映射
static void page_table_add(void *vaddr_, void *paddr_)
{
    uint32_t vaddr = (uint32_t)vaddr_;
    uint32_t paddr = (uint32_t)paddr_;
    uint32_t *pde = pde_ptr(vaddr);
    uint32_t *pte = pte_ptr(vaddr);
    //目录项存在
    if(*pde & 0x1)
    {

        ASSERT(!(*pte & 0x1));
        if(!(*pte & 0x1))
        {
            //创建前页表不存在
            *pte = (paddr | PG_US_U | PG_RW_W | PG_P_1);
        }
        else
        {
            Puts("create pte repeat");
            *pte = (paddr | PG_US_U | PG_RW_W | PG_P_1);
        }
    }
    else
    {
        uint32_t pde_addr = (uint32_t)palloc(&kernel_pool); //返回的是物理地址
        //Puts("\n");
        //Putint(pde_addr);
        //Puts("\n");
        *pde = (pde_addr | PG_US_U | PG_RW_W | PG_P_1);
        memset((void *)((uint32_t)pte & 0xfffff000), 0, PAGE_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (paddr | PG_US_U | PG_RW_W | PG_P_1);
    }
    
}

//分配size页面
void* malloc_page(enum MemoryPoolFlag flag, uint32_t size)
{
    void *vaddr_start = get_virtual_addr(flag, size);   //分配虚拟地址
    if(vaddr_start == NULL)
    {
        return NULL;
    } 
    uint32_t vaddr = (uint32_t)vaddr_start;
    uint32_t count = size;
    struct Pool* memory_pool = flag & KERNELPOOL ? &kernel_pool : &user_pool;       //申请区域是用户内存还是内核内存

    //逐个映射
    //虚拟地址连续但是物理地址不一定连续
    while(count-- > 0)
    {
        void *physical_page = palloc(memory_pool);  //在物理空间中分配一页
        if(physical_page == NULL)
        {
            return NULL;
        }
        //添加虚拟地址到物理地址的映射，更新页目录表与页表
        page_table_add((void*)vaddr, physical_page);
        vaddr += PAGE_SIZE;
    }
    return vaddr_start;
}

//从内核物理内存中申请一页
void *get_kernel_page(uint32_t size)
{
    void *vaddr = malloc_page(KERNELPOOL, size);
    if(vaddr != NULL)
    {
        memset((void*)vaddr ,0, size*PAGE_SIZE);
    }
    return vaddr;
}

void *get_user_page(uint32_t size)
{
    lock_acquire(&user_pool.lock);
    void *vaddr = malloc_page(USERPOOL, size);
    if(vaddr != NULL)
    {
        memset((void *)vaddr, 0, size*PAGE_SIZE);
    }
    lock_release(&user_pool.lock);
    return vaddr;
}

//申请一页地址，并用vaddr映射
void *get_page(enum MemoryPoolFlag flag, uint32_t vaddr)
{
    struct Pool* memory_pool = flag & KERNELPOOL ? &kernel_pool : &user_pool;       //申请区域是用户内存还是内核内存
    lock_acquire(&memory_pool->lock);
    struct TaskStruct *task = running_thread();
    int32_t index = -1;

    //用户进程
    if(task->page_addr != NULL && flag == USERPOOL)
    {
        index = (vaddr - task->user_addr.virtual_addr_start) / 4096;
        bitmap_set_musk(&task->user_addr.virtual_addr_bitmap, index, 1);
    }
    else if(task->page_addr == NULL && flag == KERNELPOOL)
    {
        index = (vaddr - kernel_virtual_addr.virtual_addr_start) / 4096;
        bitmap_set_musk(&kernel_virtual_addr.virtual_addr_bitmap, index, 1);
    }

    void *page = palloc(memory_pool); 
    if(page == NULL)
    {
        return NULL;
    }
    page_table_add((void *)vaddr, page);
    lock_release(&memory_pool->lock);
    return (void *)vaddr;
}


uint32_t addr_v2p(uint32_t vaddr)
{
    uint32_t *pte = pte_ptr(vaddr);         //找到页表地址
    return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}


void memory_desc_init(struct MemoryDesc *memory_desc)
{
    uint16_t i = 0, temp = 16;
    for(; i<7; i++)
    {
        memory_desc[i].size = temp;
        //可以分多少块
        memory_desc[i].arena_nums =  (4096 - sizeof(struct Arena)) / temp;
        list_init(&memory_desc[i].free_list);
        temp *= 2;
    }
}



void memory_init()
{
    Puts("memort init start\n");
    uint32_t memory_byte_total = *((uint32_t *)(0x700));   //之前获取的地址存在这
    lock_init(&user_pool.lock);
    lock_init(&kernel_pool.lock);
    memory_pool_init(memory_byte_total);
    memory_desc_init(kernel_memory_desc);
    Puts("memory init done\n");
}



//返回 arena 中第 idx 个内存块的首地址。
static struct MemoryBlock* area2block(struct Arena *area, uint32_t index)
{
    return (struct MemoryBlock *)((uint32_t)area + sizeof(struct Arena) + index * area->desc->size);
}
//内存块的高 20 位地址便是 arena 所在的地址
static struct Arena* block2area(struct MemoryBlock *block)
{
    struct Arena *p = (struct Arena *)((uint32_t)block & 0xfffff000);
    return p;
}

void *sys_malloc(uint32_t size)
{
    struct TaskStruct *thread = running_thread();
    enum MemoryPoolFlag flag;
    uint32_t pool_size;
    struct Pool *memory_pool;
    struct MemoryDesc *desc;
    if(thread->page_addr == (void *)0)
    {
        //内核线程
        flag = KERNELPOOL;
        pool_size = kernel_pool.memory_pool_size;
        memory_pool = &kernel_pool;
        desc = kernel_memory_desc;
    }
    else
    {
        flag = USERPOOL;
        pool_size = user_pool.memory_pool_size;
        memory_pool = &user_pool;
        desc = thread->user_memory_desc;
    }
    if(!(size > 0 && size < pool_size))
    {
        return (void *)0;
    }
    struct Arena *a;
    struct MemoryBlock *b;
    lock_acquire(&memory_pool->lock);
    if(size > 1024)
    {
        uint32_t page = (size + sizeof(struct Arena) + 4096 - 1) / 4096;
        a = malloc_page(flag, page);
        if(a)
        {
            memset(a, 0, page * 4096);
            a->large = 1;
            a->desc = (void*)0;
            a->cnt = page;
            lock_release(&memory_pool->lock);
            return (void *)(a + 1);
        }
        else
        {
            lock_release(&memory_pool->lock);
            return (void *)0;
        }
        
    }
    else
    {
        //申请的内存小于1024
        uint32_t index;
        for(index = 0; index < 7; ++index)
        {
            if(size <= desc[index].size)
            {
                break;
            }
        }
        if(list_empty(&desc[index].free_list))
        {
            //分配内存
            a = malloc_page(flag, 1);
            if(!a)
            {
                lock_release(&memory_pool->lock);
                return (void *)0;
            }
            memset(a, 0, 4096);
            a->desc = &desc[index];
            a->large = 0;
            a->cnt = desc[index].arena_nums;        //一块能分配多少
            enum InterruptStatus status = Off_interrupt();
            uint32_t i = 0;
            for(; i<desc[index].arena_nums; i++)
            {
                b = area2block(a, i);
                list_push_back(&a->desc->free_list, &b->memory_list);
            }
            set_InterruptStatus(status);
        }
        struct MemoryBlock temp_block;
        uint32_t len = (uint32_t)&temp_block.memory_list - (uint32_t)&temp_block;
        b = (struct MemoryBlock *)(list_pop(&(desc[index].free_list)) - len);

        memset(b, 0, desc[index].size);
        a = block2area(b);
        a->cnt--;               //块大小减一
        lock_release(&memory_pool->lock);
        return (void *)b;
    }  
}