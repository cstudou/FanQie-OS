#include "keyboard.h"

#define ESC             '\033'
#define BACKSPACE       '\b'
#define TAB             '\t'
#define ENTER           '\r'
#define DELETE          '\177'

//不可见字符
#define CTRL_L          0
#define CTRL_R          0
#define SHIFT_L         0
#define SHIFT_R         0
#define ALT_L           0
#define ALT_R           0
#define CAPSLOCK        0

//通码&断码
#define SHIFT_L_ON      0x2a
#define SHIFT_R_ON      0x36
#define ALT_L_ON        0x38
#define ALT_R_ON        0xe038
#define ALT_R_OFF       0xe0b8
#define CTRL_L_ON       0x1d
#define CTRL_R_ON       0xe01d
#define CTRL_R_OFF      0xe09d
#define CAPSLOCK_ON     0x3a

//scancode_flag判断通码是否是0xe0开头
static int ctrl_status, shift_status, alt_status, capslock_status, scancode_flag;
struct IoQueue keyboard_queue;

/* 以通码make_code为索引的二维数组 */
static char keymap[][2] = {
/* 扫描码   未与shift组合  与shift组合*/
/* ---------------------------------- */
/* 0x00 */	{0,	0},		
/* 0x01 */	{ESC,	ESC},		
/* 0x02 */	{'1',	'!'},		
/* 0x03 */	{'2',	'@'},		
/* 0x04 */	{'3',	'#'},		
/* 0x05 */	{'4',	'$'},		
/* 0x06 */	{'5',	'%'},		
/* 0x07 */	{'6',	'^'},		
/* 0x08 */	{'7',	'&'},		
/* 0x09 */	{'8',	'*'},		
/* 0x0A */	{'9',	'('},		
/* 0x0B */	{'0',	')'},		
/* 0x0C */	{'-',	'_'},		
/* 0x0D */	{'=',	'+'},		
/* 0x0E */	{BACKSPACE, BACKSPACE},	
/* 0x0F */	{TAB,	TAB},		
/* 0x10 */	{'q',	'Q'},		
/* 0x11 */	{'w',	'W'},		
/* 0x12 */	{'e',	'E'},		
/* 0x13 */	{'r',	'R'},		
/* 0x14 */	{'t',	'T'},		
/* 0x15 */	{'y',	'Y'},		
/* 0x16 */	{'u',	'U'},		
/* 0x17 */	{'i',	'I'},		
/* 0x18 */	{'o',	'O'},		
/* 0x19 */	{'p',	'P'},		
/* 0x1A */	{'[',	'{'},		
/* 0x1B */	{']',	'}'},		
/* 0x1C */	{ENTER,  ENTER},
/* 0x1D */	{CTRL_L, CTRL_L},
/* 0x1E */	{'a',	'A'},		
/* 0x1F */	{'s',	'S'},		
/* 0x20 */	{'d',	'D'},		
/* 0x21 */	{'f',	'F'},		
/* 0x22 */	{'g',	'G'},		
/* 0x23 */	{'h',	'H'},		
/* 0x24 */	{'j',	'J'},		
/* 0x25 */	{'k',	'K'},		
/* 0x26 */	{'l',	'L'},		
/* 0x27 */	{';',	':'},		
/* 0x28 */	{'\'',	'"'},		
/* 0x29 */	{'`',	'~'},		
/* 0x2A */	{SHIFT_L, SHIFT_L},	
/* 0x2B */	{'\\',	'|'},		
/* 0x2C */	{'z',	'Z'},		
/* 0x2D */	{'x',	'X'},		
/* 0x2E */	{'c',	'C'},		
/* 0x2F */	{'v',	'V'},		
/* 0x30 */	{'b',	'B'},		
/* 0x31 */	{'n',	'N'},		
/* 0x32 */	{'m',	'M'},		
/* 0x33 */	{',',	'<'},		
/* 0x34 */	{'.',	'>'},		
/* 0x35 */	{'/',	'?'},
/* 0x36	*/	{SHIFT_R, SHIFT_R},	
/* 0x37 */	{'*',	'*'},    	
/* 0x38 */	{ALT_L, ALT_L},
/* 0x39 */	{' ',	' '},		
/* 0x3A */	{CAPSLOCK, CAPSLOCK}
/*其它按键暂不处理*/
};



static void keyboard_intr_handler()
{
    int ctrl_last_status = ctrl_status;
    int shift_last_status = shift_status;
    int capslock_last_status = capslock_status;

    int off_code;
    uint16_t scancode = readb(0x60);  //此时为断码
    if(scancode == 0xe0)
    {
        // 0xe0表示有多个扫描码
        scancode_flag = 1;
        return ;
    }

    if(scancode_flag == 1)
    {
        scancode = scancode | 0xe000;   //将scancode与0xe0合并
        scancode_flag = 0; 
    }
    off_code = ((scancode & 0x0080) != 0);    //断码的第八位是1
    if(off_code)
    {
        //是断码
        //第一套键盘扫描码通码和断码的区别就是扫描码第 8 位的值，
        //断码的第 8 位为 1，通码 的第 8 位为 0。
        uint16_t on_code = (scancode &= 0xff7f);
        //判断通码
        //下面的判断要置位false，因为断码处键弹起来了
        if(on_code == CTRL_L_ON || on_code == CTRL_R_ON)    { ctrl_status = 0; }
        if(on_code == SHIFT_L_ON || on_code == SHIFT_R_ON)  { shift_status = 0; }
        if(on_code == ALT_L_ON || on_code == ALT_R_ON)  { alt_status = 0; }
        return ;
    }
    else if((scancode > 0x00 && scancode < 0x3b) || (scancode == ALT_R_ON) || (scancode == CTRL_R_ON))
    {
        /* 通码 */
        int shift = 0;
        //按下shift后会改变，不必一直按着
        if ((scancode < 0x0e) || (scancode == 0x29) || (scancode == 0x1a) || (scancode == 0x1b) || (scancode == 0x2b) \
        || (scancode == 0x27) || (scancode == 0x28) || (scancode == 0x33) || (scancode == 0x34) || (scancode == 0x35)) 
        {
            // 如果同时按下了 shift 键  
            if (shift_last_status)  { shift = 1; }
        }
        else
        {
            if(shift_last_status && capslock_last_status)   { shift = 0; }
            else if(shift_last_status || capslock_last_status)  { shift = 1; }
            else    { shift = 0; }
            
        }
        uint8_t index = (scancode &= 0x00ff);
        char str = keymap[index][shift];        //shift选择是哪个字符
        if(str)
        {
            //缓冲区不满
            if(!ioqueue_full(&keyboard_queue))
            {
                Putchar(str);
                ioqueue_putchar(&keyboard_queue, str);
            }
            //Putchar(str);
            return ;
        }
        //如果str是0，那么就是alt、ctrl、shift、caps
        if(scancode == CTRL_L_ON || scancode == CTRL_R_ON)  { ctrl_status = 1; }
        else if(scancode == SHIFT_R_ON || scancode == SHIFT_L_ON)   { shift_status = 1; }
        else if(scancode == CAPSLOCK_ON)    { capslock_status = !capslock_status; }
        
    }
    else
    {
        Puts("unknow key\n");
    }
}

void keyboard_init()
{
    Puts("keyboard init start!\n");
    ioqueue_init(&keyboard_queue);
    register_intr_handler(0x21, keyboard_intr_handler);
    Puts("keyboard init done!\n");
}

