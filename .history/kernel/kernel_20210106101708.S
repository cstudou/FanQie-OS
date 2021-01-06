; 本文件注册了中断处理函数完整版
; 从interrupt.c文件得到中断处理函数地址
; 返回地址用于构建中断描述符表
SELECTOR_VIDEO equ (0x0003<<3) + 000b + 00b

[bits 32]
%define ERROR_CODE nop
%define ZERO push 0

extern intr_func_table          ; 中断处理程序

section .data
global intr_entry_table
intr_entry_table:               ; 中断向量表


%macro VECTOR 2
section .text
intr%1entry:                    ; 标号，%1代表中断向量号，中断处理程序起始地址的范围是 intr[0~32]entry
    %2                          ; 没有错误码就手动压入一个0
    push ds
    push es
    push fs
    push gs
    pushad                      ; 保存上下文

    mov al, 0x20                ; EOI，End Of Interrupt，为中断结束命令位
    out 0xa0, al
    out 0x20, al
    push %1
    call [intr_func_table + %1 * 4] 
    jmp intr_exit
section .data   
    dd intr%1entry              ; 编译器会将属性相同的 section 合并到同一个大的 segment 中
%endmacro


section .text
global intr_exit
intr_exit:
    add esp, 4
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4                 ; 跳过error_code
    iretd


; 0x80 中断
[bits 32]
extern syscall_table
section .text
global syscall_handler
syscall_handler:
    push 0
    push ds
    push es
    push fs
    push gs
    pushad                      ; EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
    push 0x80

    push edx
    push ecx
    push ebx                    ; 三个参数

    call [syscall_table + eax*4]
    add esp, 12                 ; 跳过参数
    mov [esp + 32], eax         ; popad会恢复寄存器上下文，把eax写到popad中的位置
    jmp intr_exit               ; jmp指令不存返回地址

[bits 32]
section .text
global cls_screen
cls_screen:                     ; 清屏
    pushad



; 中断向量 0~19 为处理器内部固定的异常类型，20~31 是 Intel 保留的
VECTOR 0x00,ZERO
VECTOR 0x01,ZERO
VECTOR 0x02,ZERO
VECTOR 0x03,ZERO 
VECTOR 0x04,ZERO
VECTOR 0x05,ZERO
VECTOR 0x06,ZERO
VECTOR 0x07,ZERO 
VECTOR 0x08,ERROR_CODE
VECTOR 0x09,ZERO
VECTOR 0x0a,ERROR_CODE
VECTOR 0x0b,ERROR_CODE 
VECTOR 0x0c,ZERO
VECTOR 0x0d,ERROR_CODE
VECTOR 0x0e,ERROR_CODE
VECTOR 0x0f,ZERO 
VECTOR 0x10,ZERO
VECTOR 0x11,ERROR_CODE
VECTOR 0x12,ZERO
VECTOR 0x13,ZERO 
VECTOR 0x14,ZERO
VECTOR 0x15,ZERO
VECTOR 0x16,ZERO
VECTOR 0x17,ZERO 
VECTOR 0x18,ERROR_CODE
VECTOR 0x19,ZERO
VECTOR 0x1a,ERROR_CODE
VECTOR 0x1b,ERROR_CODE 
VECTOR 0x1c,ZERO
VECTOR 0x1d,ERROR_CODE
VECTOR 0x1e,ERROR_CODE
VECTOR 0x1f,ZERO 
VECTOR 0x20,ZERO                ; 时钟中断
VECTOR 0x21,ZERO                ; 键盘中断
VECTOR 0x22,ZERO	;级联用的
VECTOR 0x23,ZERO	;串口2对应的入口
VECTOR 0x24,ZERO	;串口1对应的入口
VECTOR 0x25,ZERO	;并口2对应的入口
VECTOR 0x26,ZERO	;软盘对应的入口
VECTOR 0x27,ZERO	;并口1对应的入口
VECTOR 0x28,ZERO	;实时时钟对应的入口
VECTOR 0x29,ZERO	;重定向
VECTOR 0x2a,ZERO	;保留
VECTOR 0x2b,ZERO	;保留
VECTOR 0x2c,ZERO	;ps/2鼠标
VECTOR 0x2d,ZERO	;fpu浮点单元异常
VECTOR 0x2e,ZERO	;硬盘
VECTOR 0x2f,ZERO	;保留
