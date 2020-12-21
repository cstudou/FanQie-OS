[bits 32]
section .text
global switch_to
switch_to:
    push esi
    push edi
    push ebx
    push ebp

    mov eax, [esp + 20]     ; 当前线程pcb地址
    mov [eax], esp          ; 结构体第一个参数就是栈指针
    mov eax, [esp + 24]     ; 下一个要处理的线程
    mov esp, [eax]          ; esp指向新pcb的内核栈

    pop ebp
    pop ebx
    pop edi
    pop esi
    ret


