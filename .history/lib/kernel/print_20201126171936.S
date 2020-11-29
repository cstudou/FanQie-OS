%include "head.inc"
[bits 32]
section .text

; 写一个字符
global Putchar
Putchar:
    pushad                          ; PUSHAD指令压入32位寄存器，其入栈顺序是:EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
    mov ax, SELECTOR_VIDEO
    mov gs, ax                      ; 不能把立即数送到段寄存器

    ; 获取光标位置
    ; CRT Controller Data Registers 0Eh和0Fh存光标高8位和低8位
    ; 访问 CRT controller 寄存器组的寄存器，需要先往端口地址为 0x3D4 的 Address Register 寄存器中写入 寄存器的索引，
    ; 再从端口地址为 0x3D5 的 Data Register 寄存器读、写数据

    mov dx, 0x3d4                   ; 索引寄存器
    mov ax, 0xe                     ; 光标高8位
    out dx, al
    mov dx, 0x3d5                   ;数据寄存器
    in al, dx
    mov ah, al                      ; 获得高8位

    mov dx, 0x3d4                  
    mov ax, 0xf                     ; 光标低8位
    out dx, al
    mov dx, 0x3d5                   
    in al, dx                       ; 获得低8位
    mov bx, ax                      ; bx为光标位置
    mov ecx, [esp + 36]             ; 8个寄存器1个返回地址

    cmp cl, 0xd                     ; 回车
    jz .carriage_return
    cmp cl, 0xa                     ; 换行
    jz .ine_feed
    cmp cl, 0x8                     ; 退格
    jz .backspace
    jmp .put_other


.backspace:                      ; 删除前一个字符
    dec bx
    shl bx, 1                       ; 一个字符两字节
    mov byte [gs:bx], 0x20          ; 空格
    inc bx
    mov byte [gs:bx], 0x07          ; 黑白背景
    shr bx, 1
    jmp .set_cursor

.put_other:
    shl bx, 1
    mov byte [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    cmp bx, 2000
    jl .set_cursor                  ; 一页最多写2000个字符

.line_feed:
.carriage_return:
    xor dx, dx                      ; dx是被除数的高16位
    mov ax, bx                      ; ax是被除数的低16位
    mov si, 80
    div si                          ; 商在ax余数在dx
    sub bx, dx                      ; 回车
.carriage_return_end:
    add bx, 80                      ; 换行
    cmp bx, 2000
.line_feed_end:
    jl .set_cursor


.roll_screen:                      ; 滚屏，吧1-24行数据往上搬
    cld                            ; 每次rep循环的时候,esi和edi自动+1
    mov ecx, 960                   ; 23行1920字符--3840字节，一次搬4字节搬960次

    mov esi, 0xb8a0
    mov edi, 0xb800
    rep movsd
    mov ebx, 3840
    mov ecx, 80
.clean_line:                        ; 最后一行填充空白
    mov byte [gs:ebx], 0x720
    add ebx, 2
    loop .clean_line
    mov bx, 1920

.set_cursor:
    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    mov al, bl              ; bl是光标位置
    out dx, al
.putchar_done:
    popad
    ret



