[bits 32]
section .text
global switch_to
switch_to:
    push esi
    push edi
    push ebx
    push ebp

    mov eax, [esp + 20]     ; 当前线程
    mov [eax], esp
    mov eax, [esp + 24]     ; 下一个要处理的线程
    mov esp, [eax]

    pop ebp
    pop ebx
    pop edi
    pop esi
    ret