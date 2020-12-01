[bits 32]
%define ERROR_CODE nop
%define ZERO push 0
extern Puts

section .data
    intr_message db "interrupt occur!", 0xa, 0
global intr_entry_table
intr_entry_table:               ; 中断向量表


%macro VECTOR 2
section .text
intr%1entry:                    ; 标号，%1代表中断向量号，中断处理程序起始地址的范围是 intr[0~32]entry
    %2                          ; 没有错误码就手动压入一个0
    push intr_message
    call Puts
    add esp, 4
    mov al, 0x20                ; EOI，End Of Interrupt，为中断结束命令位
    out 0xa0, al
    out 0x20, al
    add esp, 4                  ; 跳过error_code
    iret
section .data   
    dd intr%lentry              ; 编译器会将属性相同的 section 合并到同一个大的 segment 中
%macroend

