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
VECTOR 0x0c,ERROR_CODE
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
VECTOR 0x20,ZERO