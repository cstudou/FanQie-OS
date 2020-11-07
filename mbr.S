; MBR程序
; 段寄存器 DS, ES, SS, CS
; 控制寄存器 IP, flags
; 数据寄存器 AX,BX,CX,DX
; 指针寄存器 SP, BP
; 变址寄存器 SI, DI

; -------------------------
section MBR vstart=0x7c00
    mov ax, cs  ; BIOS 是通过 jmp 0：0x7c00 跳转到 MBR 的
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov sp, 0x7c00  ; 初始化栈指针

    mov ax, 0xb800  ; 显卡操作
    mov gs, ax;     ; 目标地址0xb8000，段基址0xb800

;   中断 INT 0x10 功能号 0x06 --> 功能：初始化屏幕或滚屏 AH＝06H—向上滚屏，07H—向下滚屏
;   输入:
;       AH 功能号= 0x06
;       AL = 上卷的行数(如果为 0,表示全部)
;       BH = 上卷行属性
;       (CL,CH) = 窗口左上角的(X,Y)位置
;       (DL,DH) = 窗口右下角的(X,Y)位置
    mov ax, 0x600
    mov bx, 0x700   ; 起始地址编译为 0x7c00。
    mov cx, 0x0
    mov dx, 0x184f  
    ; 右下角: (80,25),VGA文本模式中,一行只能容纳80个字符,共 25 行｡下标从 0 开始,所以 0x18=24,0x4f=79 
    ; 一个字符要用两字节来表示，低字符是字符的 ASCII 编码，高字节是字符属性
    int 0x10

;   获取光标位置，功能号 0x03，在文本坐标下，读取光标各种信息
;       AH＝03H
;       BH＝显示页码
;       CH＝光标的起始行
;       CL＝光标的终止行
;       DH＝行(Y坐标)
;       DL＝列(X坐标)

    mov ah, 0x03
    mov bh, 0
    int 0x10

;   功能号：13H,显示字符串
;       AH＝13H
;       BH＝页码
;       BL＝属性(若AL=00H或01H)
;       CX＝显示字符串长度
;       (DH、DL)＝坐标(行、列)
;   ES:BP＝显示字符串的地址 AL＝ 显示输出方式
;   0—字符串中只含显示字符，其显示属性在BL中。显示后，光标位置不变
;   1—字符串中只含显示字符，其显示属性在BL中。显示后，光标位置改变
;   2—字符串中含显示字符和显示属性。显示后，光标位置不变
;   3—字符串中含显示字符和显示属性。显示后，光标位置改变

    ;mov ax, string
    ;mov bp, ax  ; es刚开始就被初始化为cs（段基址）
    ;mov ax, 0x1301
    ;mov bx, 0x2 ; bl 中是字符属性,属性黑底绿字(bl = 02h) 
    ;mov cx, 6
    ;mov dx, 0
    ;int 0x10
    
    mov byte [gs:0x00], 'F'
    mov byte [gs:0x01], 0x24

    mov byte [gs:0x02], 'a'
    mov byte [gs:0x03], 0x24

    mov byte [gs:0x04], 'n'
    mov byte [gs:0x05], 0x24

    mov byte [gs:0x06], 'Q'
    mov byte [gs:0x07], 0x24

    mov byte [gs:0x08], 'i'
    mov byte [gs:0x09], 0x24

    mov byte [gs:0x0A], 'e'
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
    ;string db "1 MBR"
    times 510-($-$$) db 0  ; 剩余地址置0
    db 0x55, 0xaa


