#include "interrupt.h"
#define PIC_M_CTRL 0x20         //主片控制端口
#define PIC_M_DATA 0x21         //主片数据端口
#define PIC_S_CTRL 0xa0         //从片控制端口
#define PIC_S_DATA 0xa1         //从片数据端口
#define IDT_COUNT 0x21

//中断门描述符
struct InterruptGate
{
    uint16_t    func_offset_low;    //中断处理程序低16位，低32位0-15
    uint16_t    func_offset_high;   //中断处理程序高16位
    uint16_t    selector;           //中断处理程序目标代码段选择子，低32位16-31
    uint8_t     fixed;              //高32位的0-8位，固定值
    uint8_t     attribute;          //高32位的8-16位
};
static struct InterruptGate idt[IDT_COUNT];
char *intr_msg[IDT_COUNT];
void *intr_func_table[IDT_COUNT];

extern void *intr_entry_table[IDT_COUNT];
static void general_intr_handler(uint8_t name)
{
    if(name == 0x27 || name == 0x2f)        //IRQ7和IRQ15会产生伪中断(spurious interrupt)，无需处理
    {
        return ;
    }
    Puts("int vector : 0x"); 
    Putint(name);
    Putchar('\n');

}

static void exception_intr_init()
{
    int i;
   for (i = 0; i < 0x21; i++) {

/* idt_table数组中的函数是在进入中断后根据中断向量号调用的,
 * 见kernel/kernel.S的call [idt_table + %1*4] */
      intr_func_table[i] = general_intr_handler;		    // 默认为general_intr_handler。
							    // 以后会由register_handler来注册具体处理函数。
      intr_msg[i] = "unknown";				    // 先统一赋值为unknown 
   }

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
    int i;
   for (i = 0; i < 0x21; i++) {
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
    exception_intr_init();          //异常名初始化并注册通常的中断处理函数
    pic_init();                     //初始化可编程中断控制器
                                              
    uint64_t idt_data =  (sizeof(idt) - 1) | (uint64_t)((uint32_t)idt << 16);  //32位表基址，16位表界限
    asm volatile("lidt %0"::"m"(idt_data));  //%0 其实是 idt_data 的地址&idt_data,AT&T把内存寻址放在最高级，任何数字都被看成是内存地址
    Puts("idt init done!\n");
    //Puts("hello world\n");
}

