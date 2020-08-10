SECTION MBR vstate = 0x7c00    ;起始编译地址0x7c00
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov sp,0x7c00       ;初始化栈寄存器


    ;清屏,ax-0x6 屏幕上卷
    ;AL代表上卷行数，0代表全部
    mov ax,0x600     ;8086cpu每一个16位寄存器可以当两个8为用，高8位为AH，低8位为AL
    mov bx,0x700
    mov cx,0         ;窗口左上角x，y
    mov dx,0x184f    ;窗口右下角x，y,一页25行一行80字符
    int 0x10    ;中断

    mov ah,3   ;3号子功能能获取光标位置
    mov bh,0   ;bh寄存器存储待获取光标的列号
    int 0x10

	mov ax,message
	mov bp,ax
	

	mov cx,6    ;字符串长度
	mov ax,0x1301  ;13号功能显示字符和属性（AH),AL=01表示显示字符串并且光标跟随移动
	mov bx,0x2    ;BH表示显示的页号，BL表示属性
	int 0x10

	jmp $
	message db "FanQie"
	times 510-($-$$) db 0  ;最后两字节必须是0x55与0xaa，剩下的字节用0填满
	db 0x55,0xaa
	
	
