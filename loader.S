%include "boot.inc"
section loader vstart=LOADER_BASE_ADDR
    mov byte [gs:0x00], 'S'
    mov byte [gs:0x01], 0x24

    mov byte [gs:0x02], 't'
    mov byte [gs:0x03], 0x24

    mov byte [gs:0x04], 'a'
    mov byte [gs:0x05], 0x24

    mov byte [gs:0x06], 'r'
    mov byte [gs:0x07], 0x24

    mov byte [gs:0x08], 't'
    mov byte [gs:0x09], 0x24

    mov byte [gs:0x0A], '.'
    mov byte [gs:0x0B], 0x24

    ; 功能02H
    ; 功能描述：用文本坐标下设置光标位置
    ;入口参数：AH＝02H
    ;BH＝显示页码
    ;DH＝行(Y坐标)
    ;DL＝列(X坐标)
    mov ax, 0x200
    mov bx, 0
    mov dx, 0x6
    int 0x10
    
    jmp $
    times 512-($-$$) db 0  ; 剩余地址置0
    