; MBR程序
; 段寄存器 DS, ES, SS, CS
; 控制寄存器 IP, flags
; 数据寄存器 AX,BX,CX,DX
; 指针寄存器 SP, BP
; 变址寄存器 SI, DI

; -------------------------
%include "boot.inc"
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


    ; 读取硬盘n个扇区
    mov eax, LOADER_STARTS_SECTOR    ; 起始扇区
    mov bx, LOADER_BASE_ADDR         ; 写入的地址
    mov cx, 1                        ; 待写入的扇区数
    
    call read_disk
    jmp LOADER_BASE_ADDR

read_disk:
    mov esi, eax                    ; 复制eax
    mov di, cx                      
    ; 1.设置要读区的扇区数
    mov dx, 0x1f2                   ; 0x1f2端口指定读取或写入的扇区数
    mov al, cl
    out dx, al                      ; 读取的扇区数
    mov eax, esi

    ; 2.存储LBA24地址
    mov dx, 0x1f3
    out dx, al
    
    mov cl, 8
    shr eax, cl                     ; 逻辑右移
    mov dx, 0x1f4
    out dx, al

    shr eax, cl
    mov dx, 0x1f5
    out dx, al

    shr eax, cl
    and al, 0x0f                    ; LBA24最后四位
    or al, 0xe0                    ; 设置7-4位为1110，表示lba模式
    mov dx, 0x1f6
    out dx, al

    ; 3.向0xf17端口写入读命令0x20
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

; 4.检测硬盘状态
.not_ready:
    nop                             ; 空指令
    in al, dx                       ; 读端口
    and al, 0x88                    ; 第 4 位为 1 表示硬盘控制器已准备好数据传输
                                    ; 第 7 位为 1 表示硬盘忙
    cmp al, 0x08
    jnz .not_ready                  ; 0标志（ZF）没有被置位

    ; 5.从0x1f0读取数据
    mov ax, di                      ; 读一个扇区
    mov dx, 256                     ; 每次读入一个字（2字节）
    mul dx                          ; dx * ax = (低16位存在ax，高16位存dx)
    mov cx, ax
    mov dx, 0x1f0
.to_read:
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .to_read                   ; 循环次数由计数寄存器cx控制
    ret


    times 510-($-$$) db 0  ; 剩余地址置0
    db 0x55, 0xaa

    

