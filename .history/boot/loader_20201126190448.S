%include "boot.inc"
section loader vstart=LOADER_BASE_ADDR
    LOADER_STACK_TOP equ LOADER_BASE_ADDR  ; 栈顶，伪指令不占空间
    
    ; 一个描述符项占8字节
    GDT_BASE: dd 0x00000000 
                dd 0x00000000

    ; 代码段描述符
    CODE_DESC: dd 0x0000FFFF 
                dd DESC_CODE_HIGH4
    ; 数据段和栈段描述符
    DATA_STACK_DESC: dd 0x0000FFFF 
                        dd DESC_DATA_HIGH4
    ; 显存段描述符
    ; limit=（0xbffff-0xb8000）/ 4k=0x7        
    VIDEO_DESC: dd 0x80000007 
                dd DESC_VIDEO_HIGH4

    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1 ; GDT最大长度
    times 60 dq 0 ; 60个描述符


    ; 构建选择子
    SELECTOR_CODE equ (0x0001<<3) + TI_GDT + RPL0
    SELECTOR_DATA equ (0x0002<<3) + TI_GDT + RPL0
    SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT + RPL0

    ; 4个段描述符 32字节，60个空描述符 480字节， 共512字节 = 0x200
    ; 定义全局描述符表的指针，起始地址-界限
    total_memory_bytes dd 0	
    gdt_ptr dw GDT_LIMIT 
            dd GDT_BASE
    
    
    ; total_memory_bytes 4字节 + gdt_ptr 6字节 + ards_buf 244字节 + ards_num 2字节, 共256字节
    ards_buf times 244 db 0
    ards_num dw 0		      ; 用于记录ards结构体数量


loader_start:
    xor ebx, ebx		      ; 第一次调用时，ebx值要为0
    mov edx, 0x534d4150	      ; 魔数SMAP
    mov di, ards_buf	      ; ards结构缓冲区
.get_memory_E820:	          ; 使用E820获取
    mov eax, 0x0000e820	      ; 执行int 0x15后,eax值变为0x534d4150,所以每次执行int前都要更新为子功能号。
    mov ecx, 20		          ; ARDS地址范围描述符结构大小是20字节
    int 0x15
    jc .get_memory_E801       ; 若cf位为1则有错误发生，尝试0xe801子功能
    add di, cx		            
    inc word [ards_num]	      ; 记录ARDS数量
    cmp ebx, 0		          ; 若ebx为0且cf不为1,这说明ards全部返回，当前已是最后一个
    jnz .get_memory_E820

    mov cx, [ards_num]	      ; 遍历每一个ARDS结构体,循环次数是ARDS的数量
    mov ebx, ards_buf 
    xor edx, edx		      ; edx为最大的内存容量
.find_max_memory_area:	      
    mov eax, [ebx]	     
    add eax, [ebx+8]	      
    add ebx, 20		          ; 指向缓冲区中下一个ARDS结构
    cmp edx, eax		      ; edx寄存器始终是最大的内存容量
    jge .next_ards
    mov edx, eax		      
.next_ards:
    loop .find_max_memory_area
    jmp .get_memory_ok


.get_memory_E801:               ; 使用E801获取，在ax和cx寄存器中为低16M,在bx和dx寄存器中为16MB到4G。
    mov ax,0xe801
    int 0x15
    jc .get_memory_0x88   ;若当前e801方法失败,就尝试0x88方法

    ; 先算出低15M的内存,ax和cx中是以KB为单位的内存数量,将其转换为以byte为单位
    mov cx,0x400	     
    mul cx 
    shl edx,16
    and eax,0x0000FFFF
    or edx,eax
    add edx, 0x100000           ; ax只是15MB,故要加1MB
    mov esi,edx	   

    ; 再将16MB以上的内存转换为byte为单位,寄存器bx和dx中是以64KB为单位的内存数量
    xor eax,eax
    mov ax,bx		
    mov ecx, 0x10000	
    mul ecx		                ; 32位乘法,默认的被乘数是eax,积为64位,高32位存入edx,低32位存入eax.
    add esi,eax		
    mov edx,esi		
    jmp .get_memory_ok

.get_memory_0x88:               ; 使用0x88获取
    mov  ah, 0x88
    int  0x15
    jc .get_memory_error
    and eax,0x0000FFFF
      
    ; 16位乘法，被乘数是ax,积为32位积的高16位在dx中，积的低16位在ax中
    mov cx, 0x400     
    mul cx
    shl edx, 16	     
    or edx, eax	     
    add edx,0x100000  ; 0x88子功能只会返回1MB以上的内存,故实际内存大小要加上1MB

.get_memory_error:
    hlt
.get_memory_ok:
    mov [total_memory_bytes], edx	 ;将内存换为byte单位后存入total_memory_bytes处。
    ; 打开A20
    in al, 0x92
    or al, 00000010B
    out 0x92, al

    ; 加载GDT
    lgdt [gdt_ptr]


    ; 进入保护模式
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax

    jmp dword SELECTOR_CODE:p_mode_start
    ;jmp dword SELECTOR_CODE:enter_kernel

[bits 32]
enter_kernel:
    call kernel_init
    mov esp, 0xc009f000          ; 映射为0x9f000
    
    jmp KERNEL_ENTRY_POINT



; 创建页目录及页表 
[bits 32]
setup_page:
   mov ecx, 4096                ; 清0
   mov esi, 0
.clear_page_dir:
   mov byte [PG_DIR_POS + esi], 0
   inc esi
   loop .clear_page_dir

;开始创建页目录项(PDE)
.create_pde:				     ; 创建Page Directory Entry(页目录表)
   mov eax, PG_DIR_POS
   add eax, 0x1000 			     ; 第一个页表的位置及属性
   mov ebx, eax				    

   or eax, PG_US_U | PG_RW_W | PG_P	    ; 页目录项的属性RW和P位为1,US为1,表示用户属性,所有特权级别都可以访问.
   mov [PG_DIR_POS + 0x0], eax           
   mov [PG_DIR_POS + 0xc00], eax        ; 0xc00表示第768个页表占用的目录项,0xc00以上的目录项用于内核空间,
					                    ; 页表的0xc0000000~0xffffffff共计1G属于内核,0x0~0xbfffffff共计3G属于用户进程.
   sub eax, 0x1000
   mov [PG_DIR_POS + 4092], eax	        ; 使最后一个目录项指向页目录表自己的地址

   mov ecx, 256				            ; 1M低端内存 共256个页表项
   mov esi, 0
   mov edx, PG_US_U | PG_RW_W | PG_P	     
.create_pte:				            ; 创建Page Table Entry（页表项）
   mov [ebx+esi*4],edx			        ; 此时的ebx已经在上面通过eax赋值为0x101000,也就是第一个页表的地址 
   add edx,4096
   inc esi
   loop .create_pte

   mov eax, PG_DIR_POS
   add eax, 0x2000 		                ; 此时eax为第二个页表的位置
   or eax, PG_US_U | PG_RW_W | PG_P
   mov ebx, PG_DIR_POS
   mov ecx, 254			                ; 范围为第769~1022的所有目录项数量，最后一个页目录项是指向自己
   mov esi, 769
.create_kernel_pde:
   mov [ebx+esi*4], eax
   inc esi
   add eax, 0x1000
   loop .create_kernel_pde
   ret


[bits 32]
p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, SELECTOR_VIDEO
    mov gs, ax

    mov eax, KERNEL_START_SECTOR
    mov ebx, KERNEL_BIN_ADDR
    mov ecx, 200                              ; 读200个内核扇区
    call read_disk_32
    ; 创建页目录及页表并初始化页内存位图
    call setup_page
    sgdt [gdt_ptr]	      

    mov ebx, [gdt_ptr + 2]                     ; gdt_ptr的高4位是GDT地址
    or dword [ebx + 0x18 + 4], 0xc0000000      ; 视频段是第3个段描述符,每个描述符是8字节,故0x18，高4字节的最高位是段基址的31~24位

    add dword [gdt_ptr + 2], 0xc0000000        ; 将gdt的基址加上0xc0000000使其成为内核所在的高地址

    add esp, 0xc0000000      

    ; 把页目录地址赋给cr3
    mov eax, PG_DIR_POS
    mov cr3, eax

    ; 打开cr0的pg位(第31位)
    ; 第0位是保护模式的开关
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lgdt [gdt_ptr]       
    jmp SELECTOR_CODE:enter_kernel


[bits 32]
kernel_init:
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    mov dx, [KERNEL_BIN_ADDR + 42]      ; +42处为程序头表条目大小
    mov ebx, [KERNEL_BIN_ADDR + 28]     ; +28处为程序头表在文件中的偏移
    add ebx, KERNEL_BIN_ADDR
    mov cx, [KERNEL_BIN_ADDR + 44]      ; +44处为ELF文件有多少个段
.each_segment:
    cmp byte [ebx], PT_NULL
    je .PT_NULL
    push dword [ebx + 16]               ; 该段大小
    mov eax, [ebx + 4]                  ; 本段在文件内的起始偏移字节
    add eax, KERNEL_BIN_ADDR            ; 物理地址

    push eax                            ; 源地址
    push dword [ebx + 8]                ; 目的地址
    call mem_cpy
    add esp, 12
.PT_NULL:
    add ebx, edx                        ; 跨国该段
    loop .each_segment
    ret

[bits 32]
mem_cpy:
    cld                             ; 方向标志位 DF 置为 0,esi edi自动+1
    push ebp
    mov ebp, esp
    push ecx

    mov edi, [ebp + 8]
    mov esi, [ebp + 12]
    mov ecx, [ebp + 16]
    rep movsb                       ; 按ecx重复
    pop ecx
    pop ebp
    ret


[bits 32]
read_disk_32:
    mov esi, eax                    ; 复制eax
    mov di, cx                      
    ; 1.设置要读区的扇区数
    mov dx, 0x1f2                   ; 0x1f2端口指定读取或写入的扇区数
    mov al, cl
    out dx, al                      ; 读取的扇区数写到0x1f2端口
    mov eax, esi

    ; 2.存储LBA24地址 LBALOW - LBAMID - LBAHIGH
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

    ; 3.向0xf17端口写入读命令0x20，写0x30，磁盘识别0xEC
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
    mov [ebx], ax
    add ebx, 2
    loop .to_read                   ; 循环次数由计数寄存器cx控制
    ret
