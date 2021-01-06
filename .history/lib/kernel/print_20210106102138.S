%include "lib/include/head.inc"
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
    mov al, 0xe                     ; 光标高8位
    out dx, al
    mov dx, 0x3d5                   ; 数据寄存器
    in al, dx
    mov ah, al                      ; 获得高8位

    mov dx, 0x3d4                  
    mov al, 0xf                     ; 光标低8位
    out dx, al
    mov dx, 0x3d5                   
    in al, dx                       ; 获得低8位
    mov bx, ax                      ; bx为光标位置
    mov ecx, [esp + 36]             ; 8个寄存器1个返回地址

    cmp cl, 0xd                     ; 回车
    jz .carriage_return
    cmp cl, 0xa                     ; 换行
    jz .line_feed
    cmp cl, 0x8                     ; 退格
    jz .backspace
    jmp .put_other


.backspace:                         ; 删除前一个字符
    dec bx
    shl bx, 1                       ; 一个字符两字节
    mov dword [gs:bx], 0x0720       ; 空格,黑白背景
    shr bx, 1
    jmp .set_cursor

.put_other:
    shl bx, 1
    mov byte [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    inc bx                          ; 光标位置+1
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

    mov esi, 0xb80a0
    mov edi, 0xb8000
    rep movsd
    mov ebx, 3840
    mov ecx, 80
.clean_line:                        ; 最后一行填充空白
    mov word [gs:ebx], 0x720        ; 空格+黑白背景
    add ebx, 2
    loop .clean_line
    mov bx, 1920

.set_cursor:
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    mov al, bh              ; bl是光标位置
    out dx, al

    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    mov al, bl              ; bl是光标位置
    out dx, al
.putchar_done:
    popad
    ret




[bits 32]
section .text
global Puts
Puts:
    pushad
    xor ecx, ecx
    mov ebx, [esp + 36]         ; 字符地址
.read_str:
    mov cl, [ebx]               ; 压入到栈中的是存储该字符串的内存首地址
    cmp cl, 0
    jz .str_end
    push ecx
    call Putchar                ; 打印一个字符
    add esp, 4
    inc ebx
    jmp .read_str
.str_end:
    popad
    ret


[bits 32]
section .Data
    put_int_buffer dq 0         ; int数据缓冲区


[bits 32]
section .text
global Putint
Putint:
    pushad
    mov ebp, esp
    mov eax, [ebp + 36]
    mov edx, eax                ; edx为int值
    mov edi, 7                  ; 在buffer中的偏移
    mov ecx, 8
    mov ebx, put_int_buffer
.read_int_hexadecimal:          ; 16字节转成4字节
    and edx, 0x0000000F
    cmp edx, 9
    jg .hexadecimal_A2F
    add edx, '0'
    jmp .store_hexadecimal
.hexadecimal_A2F:               ; 十六进制的A-F
    sub edx, 10
    add edx, 'A'
.store_hexadecimal:
    mov [ebx + edi], dl         ; 存数字，小端存在edx，在buffer中大端存
    dec edi                     ; 最终变成-1
    shr eax, 4
    mov edx, eax
    loop .read_int_hexadecimal
.convert_to_standard:
    inc edi
.skip_zero:
    cmp edi, 8
    jz .all_zero
    mov al, [ebx + edi]
    inc edi
    cmp al, '0'
    jz .skip_zero
    dec edi                     ; 不为0
    jmp .put_int_num
.all_zero:                      ; 全部都是0
    mov al, '0'
.put_int_num:
    push eax                    ; 待打印数字
    call Putchar
    add esp, 4
    inc edi
    mov al, [ebx + edi]
    cmp edi, 8
    jl .put_int_num             ; 小于8跳转
    popad
    ret


[bits 32]
section .text
global set_cursor
set_cursor:
    pushad
    mov bx, [esp + 4 + 32]
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    mov al, bh              ; bl是光标位置
    out dx, al

    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    mov al, bl              ; bl是光标位置
    out dx, al
    popad
    ret



[bits 32]
section .text
global cls_screen
cls_screen:                     ; 清屏
    pushad
    mov ax, SELECTOR_VIDEO
    mov gx, ax
    mov ebx, 0
    mov ecx, 80*25
.clear
    mov word[gs:ebx], 0x0720
    add ebx, 2
    loop .cls
    mov ebx, 0
.set_cursor:
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    mov al, bh              ; bl是光标位置
    out dx, al

    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    mov al, bl              ; bl是光标位置
    out dx, al
    popad
    ret


