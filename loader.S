    %include "boot.inc"
    section loader vstart=LOADER_BASE_ADDR
    LOADER_STACK_TOP equ LOADER_BASE_ADDR  ; 栈顶，伪指令不占空间
    jmp loader_start    ; jmp完之后就是GDT所在地址空间

    ; 一个描述符项占8字节
    GDT_BASE: dd 0x00000000 
                dd 0x00000000

    ; 代码段描述符
    CODE_DESC: dd 0x0000FFFF 
                dd DESC_CODE_HIGH4
    ; 数据段和栈段描述符
    DATA_STACK_DESC: dd 0x0000FFFF 
                        dd DESC_DATA_HIGH4
    ; 显存段描述符
    ; limit=（0xbffff-0xb8000）/ 4k=0x7        
    VIDEO_DESC: dd 0x80000007 
                dd DESC_VIDEO_HIGH4

    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1 ; GDT最大长度
    times 60 dq 0 ; 60个描述符


    ; 构建选择子
    SELECTOR_CODE equ (0x0001<<3) + TI_GDT + RPL0
    SELECTOR_DATA equ (0x0002<<3) + TI_GDT + RPL0
    SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT + RPL0

    ; 定义全局描述符表的指针，起始地址-界限
    gdt_ptr dw GDT_LIMIT 
            dd GDT_BASE

    loadermsg db 'loader in real.'

    loader_start:
    ; 打印字符串
    mov sp, LOADER_BASE_ADDR
    mov bp, loadermsg
    mov ax, 0x1301
    mov bx, 0x001f
    mov cx, 15
    mov dx, 0x1800
    int 0x10



    mov byte [gs:0x00], 'S'
    mov byte [gs:0x01], 0x07

    mov byte [gs:0x02], 't'
    mov byte [gs:0x03], 0x07

    mov byte [gs:0x04], 'a'
    mov byte [gs:0x05], 0x07

    mov byte [gs:0x06], 'r'
    mov byte [gs:0x07], 0x07

    mov byte [gs:0x08], 't'
    mov byte [gs:0x09], 0x07

    mov byte [gs:0x0A], '.'
    mov byte [gs:0x0B], 0x07

    ; 功能02H
    ; 功能描述：用文本坐标下设置光标位置
    ;入口参数：AH＝02H
    ;BH＝显示页码
    ;DH＝行(Y坐标)
    ;DL＝列(X坐标)
    ;mov ax, 0x200
    ;mov bx, 0
    ;mov dx, 0x6
    ;int 0x10

    ; 打开A20
    in al, 0x92
    or al, 00000010B
    out 0x92, al

    ; 加载GDT
    lgdt [gdt_ptr]

    ; 进入保护模式
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax

    jmp dword SELECTOR_CODE:p_mode_start

[bits 32]
    p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, SELECTOR_VIDEO
    mov gs, ax
    mov byte [gs:160], 'P'


    jmp $


section print_head_message
