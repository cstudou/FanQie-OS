#include "interrupt.h"
#define PIC_M_CTRL 0x20         //主片控制端口
#define PIC_M_DATA 0x21         //主片数据端口
#define PIC_S_CTRL 0xa0         //从片控制端口
#define PIC_S_DATA 0xa1         //从片数据端口
#define IDT_COUNT 33
#define EFLAGS_IF 0x00000200    //eflags if位为1, IF 位于 eflags 中的第 9 位，

//中断门描述符
struct InterruptGate
{
    uint16_t    func_offset_low;    //中断处理程序低16位，低32位0-15
    
    uint16_t    selector;           //中断处理程序目标代码段选择子，低32位16-31
    uint8_t     fixed;              //高32位的0-8位，固定值
    uint8_t     attribute;          //高32位的8-16位
    uint16_t    func_offset_high;   //中断处理程序高16位
};
static struct InterruptGate idt[IDT_COUNT];
char *intr_msg[IDT_COUNT];
void *intr_func_table[IDT_COUNT];

extern void *intr_entry_table[IDT_COUNT];       //中断描述符表
static void general_intr_handler(uint8_t name)
{
    if(name == 0x27 || name == 0x2f)        //IRQ7和IRQ15会产生伪中断(spurious interrupt)，无需处理
    {
        return ;
    }
    set_cursor(0);
    int cursor_pos = 0;
    while(cursor_pos < 320)
    {
        Putchar(' ');
        cursor_pos++;
    }
    set_cursor(0);
    Puts("!!!!!!      exception message begin      !!!!!!\n");
    set_cursor(88);
    Puts(intr_msg[name]);
    if(name == 14)
    {
        //缺页，打印地址
        int page_fault_vaddr = 0;
        asm ("movl %%cr2, %0" : "=r" (page_fault_vaddr));
// cr2是存放造成page_fault的地址 
        Puts("\npage fault addr is ");
        Putint(page_fault_vaddr);
    }
    Puts("\n!!!!!!      exception message end      !!!!!!\n");
    //Puts("int vector : 0x"); 
    //Putint(name);
    //Putchar('\n');
    while(1) ;
}

static void exception_intr_init()
{
    int i=0;
    for(; i<IDT_COUNT; ++i)
    {
        intr_func_table[i] = general_intr_handler;
        intr_msg[i] = "unknown";
    }
    intr_msg[0] = "#DE Divide Error";
    intr_msg[1] = "#DB Debug Exception";
    intr_msg[2] = "NMI Interrupt";
    intr_msg[3] = "#BP Breakpoint Exception";
    intr_msg[4] = "#OF Overflow Exception";
    intr_msg[5] = "#BR BOUND Range Exceeded Exception";
    intr_msg[6] = "#UD Invalid Opcode Exception";
    intr_msg[7] = "#NM Device Not Available Exception";
    intr_msg[8] = "#DF Double Fault Exception";
    intr_msg[9] = "Coprocessor Segment Overrun";
    intr_msg[10] = "#TS Invalid TSS Exception";
    intr_msg[11] = "#NP Segment Not Present";
    intr_msg[12] = "#SS Stack Fault Exception";
    intr_msg[13] = "#GP General Protection Exception";
    intr_msg[14] = "#PF Page-Fault Exception";
    // intr_msg[15] 第15项是intel保留项，未使用
    intr_msg[16] = "#MF x87 FPU Floating-Point Error";
    intr_msg[17] = "#AC Alignment Check Exception";
    intr_msg[18] = "#MC Machine-Check Exception";
    intr_msg[19] = "#XF SIMD Floating-Point Exception";
}

static void make_idt(struct InterruptGate *interrupt_gate, uint8_t attritube, void* function)
{
    interrupt_gate->func_offset_low = (uint32_t)function & 0x0000ffff;
    interrupt_gate->func_offset_high = ((uint32_t)function >> 16) & 0x0000ffff;
    interrupt_gate->fixed = 0;
    interrupt_gate->attribute = attritube;
    interrupt_gate->selector = SELECTOR_CODE;
}

static void idt_desc_init()
{
    int i=0;
    for(; i<IDT_COUNT; ++i)
    {
        make_idt(&idt[i], IDT_DESCRIPTOR_ATTRITUBE_DPL0, intr_entry_table[i]);
    }
    Puts("idt descriptor init done!\n");
}


static void pic_init()
{
    //个人电脑中只有两片 8259A 芯片
    outb(PIC_M_CTRL, 0x11); //ICW1 用来初始化 8259A 的连接方式和中断信号的触发方式
    outb(PIC_M_DATA, 0x20); //ICW2 用来设置起始中断向量号,0x20
    outb(PIC_M_DATA, 0x04); //ICW3: IR2 接从片
    outb(PIC_M_DATA, 0x01); //ICW4: 8086 模式, 手动结束中断
    //初始化从片
    outb(PIC_S_CTRL, 0x11);
    outb(PIC_S_DATA, 0x28); //ICW2 用来设置起始中断向量号,0x28，一片8个口
    outb(PIC_S_DATA, 0x02); //ICW3: 设置从片连接到主片的 IR2 引脚
    outb(PIC_S_DATA, 0x01); //ICW4: 8086 模式, 手动结束中断
    outb(PIC_M_DATA, 0xfe); //OCW1:只不屏蔽一个信号
    outb(PIC_S_DATA, 0xff); //从片上的所有外设都屏蔽
    Puts("pic init done!\n");
}

void idt_init()
{
    Puts("idt init start!\n");
    idt_desc_init();
    pic_init();                     //初始化可编程中断控制器
    exception_intr_init();          //异常名初始化并注册通常的中断处理函数                                          
    uint64_t idt_data =  (sizeof(idt) - 1) | (uint64_t)((uint32_t)idt << 16);  //32位表基址，16位表界限
    asm volatile("lidt %0"::"m"(idt_data));  //%0 其实是 idt_data 的地址&idt_data,AT&T把内存寻址放在最高级，任何数字都被看成是内存地址
    Puts("idt init done!\n");
    //Puts("hello world\n");
}

enum InterruptStatus interrupt_get_status()
{
    uint32_t eflags = 0;
    //pushfl保存eflags
    asm volatile("pushfl; popl %0":"=g"(eflags));
    return (eflags & EFLAGS_IF) ? InterruptOn : InterruptOff;
}
//开中断，返回之前的状态
enum InterruptStatus On_interrupt()
{
    enum InterruptStatus status;
    if(interrupt_get_status() == InterruptOn)
    {
        status = InterruptOn;
        return status;
    }
    status = InterruptOff;
    asm volatile("sti":::"memory"); //保存状态,开中断
    return status;
}

enum InterruptStatus Off_interrupt()
{
    enum InterruptStatus status;
    if(interrupt_get_status() == InterruptOff)
    {
        status = InterruptOff;
        return status;
    }
    status = InterruptOn;
    asm volatile("cli":::"memory");
    return status;
}

//设置中断状态
enum InterruptStatus set_InterruptStatus(enum InterruptStatus status)
{
    if(status & InterruptOn)
    {
        return On_interrupt();
    }
    return Off_interrupt();
}

void register_intr_handler(uint8_t index, void (*function)(void *))
{
    intr_func_table[index] = function;
}