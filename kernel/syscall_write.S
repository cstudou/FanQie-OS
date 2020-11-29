section .data
    str_c_lib db "c library says: hello world!", 0xa    ; 0xa = \n
    str_c_lib_len equ $ - str_c_lib

    str_syscall db "syscall says: hello world!", 0xa
    str_syscall_len equ $ - str_syscall

section .text
global _start                       ; global 将符号导出为全局属性，对程序中的所有文件可见
_start:
    ;push str_c_lib_len
    ;push str_c_lib
    ;push 1

    ;call simu_write
    ;add esp, 12
    ;mov eax, 1
    ;int 0x80
    mov eax, 4
    mov ebx, 1
    mov ecx, str_syscall
    mov edx, str_syscall_len
    int 0x80
    mov eax, 1
    int 0x80

;simu_write:
;    push ebp
;    mov ebp, esp
;    mov eax, 4                      ; 4号功能是write
;    mov ebx, [ebp + 8]
;    mov ecx, [ebp + 12]
;    mov edx, [ebp + 16]
;    int 0x80
;    pop ebp
;    ret