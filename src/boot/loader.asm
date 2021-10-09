org 0x9200

BOOT_STACK equ 0x7c00
FAT_ENTRY_ADDR equ 0x8000
FAT_ENTRY_ADDR_SEGMENT equ 0x800

jmp loader_entry

[SECTION .s16]
[BITS 16]

loader_entry:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, BOOT_STACK
    mov bp, sp

    call clear_screen
    call reset_focus

    cli
    call open_a20_bus

    db 0x66
    lgdt [GDTR_TEMP]

    mov eax, cr0
    or eax, 1
    mov cr0, eax          ; 开启保护模式

    mov eax, cr0
    and al, 11111110b
    mov cr0, eax          ; 返回实模式
    sti

    call reset_floppy     ; 重置软驱

    call load_fat_entry

    push 9
    push LOADING_MESSAGE
    call show_message
    add sp, 4


    jmp hlt_loop

    mov ax, 1 * 8         ; 重新设置段寄存器
    mov ds, ax            ; DS, ES, FS, GS, SS都指向GDT中的内核数据段描述符
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp protect_mode                ; 切换到保护模式，指令解释变化，必须立马跳转

;---------------------------------------------
;进入保护模式
protect_mode:
    mov ax, 1 * 8         ; 重新设置段寄存器
    mov ds, ax            ; DS, ES, FS, GS, SS都指向GDT中的内核数据段描述符
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;sti

    jmp hlt_loop

; show_message(char* str, word size);
show_message:
    push bp
    mov bp, sp
    push ax
    push bx
    push dx
    push cx
    push es
    mov ax, ds
    mov es, ax
    mov ax, 1301h
    mov bx, 000fh
    mov dx, 0000h
    mov cx, [bp + 6]    ; 字符串长度
    mov bp, [bp + 4]    ; es:bp字符串地址
    int 10h
    pop es
    pop cx
    pop dx
    pop bx
    pop ax
    pop bp
    ret

reset_focus:
    push ax
    push bx
    push dx
    mov ax, 0200h
    mov bx, 0000h
    mov dx, 0000h
    int 10h
    pop dx
    pop bx
    pop ax
    ret

clear_screen:
    push ax
    push bx
    push cx
    push dx
    mov ax, 0600h
    mov bx, 0700h
    mov cx, 0
    mov dx, 0184fh
    int 10h
    pop dx
    pop cx
    pop bx
    pop ax
    ret
    
open_a20_bus:
    push bp
    mov bp, sp

    push ax
    in al, 92h
    or al, 00000010b
    out    92h, al              ; 利用A20快速门开启总线
    pop    ax

    mov sp, bp
    pop bp
    ret

wait_keyboard:
    in al, 0x64                 ; 从0x64端口读取数据
    and al, 0x2                 ; 测试i8042输入缓冲区是否为空
    jnz wait_keyboard           ; 直到输入缓冲区为空为止
    ret

close_external_interrupt:
    push bp
    mov bp, sp

    mov al, 0xff
    out 0x21, al                ; 屏蔽主pic中断

    nop                         ; 连续out可能会出错

    out 0xa1, al                ; 屏蔽从pic中断

    mov sp, bp
    pop bp
    ret

reset_floppy:
    push bp
    mov bp, sp
    push ax
    push dx

    mov ax, 0
    mov dx, 0
    int 13

    pop dx
    pop ax
    mov sp, bp
    pop bp
    ret

load_fat_entry:
    push bp
    mov bp, sp
    push si
    push di
    push bx

    mov si, 1
    mov di, 9
    mov bx, FAT_ENTRY_ADDR_SEGMENT

label_load_fat_entry_loop:
    cmp si, di
    ja label_load_fat_entry_final

    push bx
    push si
    call read_sector
    add sp, 4
    inc si
    add bx, 0x20
    jmp label_load_fat_entry_loop

label_load_fat_entry_final:
    pop bx
    pop di
    pop si
    mov sp, bp
    pop bp
    ret

; read_sector(word sector_num, char* addr);
read_sector:
    push bp
    mov bp, sp
    push bx
    push cx
    push dx

    mov ax, [bp + 6]
    mov es, ax          ; 目的内存地址段

    mov ax, [bp + 4]
    mov bl, 18
    div bl

    mov bl, ah
    inc bl
    mov cl, bl          ; 扇区

    mov bl, al
    and bl, 1
    mov dh, bl          ; 磁头

    mov bl, al
    shr bl, 1
    mov ch, bl          ; 柱面

    mov bx, 0           ; 读取地址 es:bx
    mov dl, 0           ; 驱动器A

label_reading_retry:
    mov ah, 0x02        ; 读盘指令
    mov al, 0x1         ; 读一个扇区
    int 0x13            ; 调用bios
    jc label_reading_retry

    pop dx
    pop cx
    pop bx
    mov sp, bp
    pop bp
    ret

hlt_loop:
    hlt
    jmp hlt_loop

; 字符串
LOADING_MESSAGE:
    db "laading..", 0

GDT_TEMP:
    dw 0x0000, 0x0000, 0x0000, 0x0000   ; 空描述符
    dw 0xffff, 0x0000, 0x9200, 0x00cf   ; 可读写数据段, 段限长4G-1, 段基址0, 内核数据段
    dw 0xffff, 0x0000, 0x9a28, 0x0047   ; 可读可执行代码段, 段限长512KB，段基址为0x280000, 内核代码段
    DW 0

GDTR_TEMP:
    dw 8 * 3 - 1                     ; GDT限长
    dd GDT_TEMP                      ; GDT基址
    dd 0x00