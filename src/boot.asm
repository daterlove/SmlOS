org 0x7c00

BOOT_STACK equ 0x7c00
ROOT_DIR_SECTOR_START_INDEX equ 19
ROOT_DIR_SECTOR_NUMBER equ 14

disk_info:
    jmp entry
    nop
    db "SML-OS  "      ; 启动区的名称,8字符
    dw 512             ; 每扇区字节数
    db 1               ; 每簇扇区数
    dw 1               ; FAT文件分配表之前的引导扇区
    db 2               ; FAT表的个数
    dw 224             ; 根目录 文件最大数
    dw 2880            ; 逻辑 扇区 总数
    db 0xf0            ; 磁盘种类
    dw 9               ; 每个FAT占用 多少扇区
    dw 18              ; 每磁道 扇区数
    dw 2               ; 磁头数
    dd 0               ; 隐藏扇区数
    dd 2880            ; 如果逻辑扇区总数为0，则在这里记录扇区总数
    db 0               ; 中断13的驱动器号
    db 0               ; 未使用
    db 0x29            ; 扩展引导标志
    dd 0Xffffffff      ; 卷序列号
    db "SML-OS     "   ; 卷标, 11个字符
    db "FAT12   "      ; 文件系统类型，8个字符

entry:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, BOOT_STACK
    mov bp, sp
    sub sp, 6

    call clear_screen
    call reset_focus

    push 9
    push BOOT_MESSAGE
    call show_message
    add sp, 4

    call reset_floppy

    push 0x820
    push 19
    call read_sector
    add sp, 4
    jmp hlt_loop

    ; [bp + 0] sector_start_index
    ; [bp + 2]  sector_index
    mov word [bp], ROOT_DIR_SECTOR_START_INDEX
    mov word [bp + 2], 0

sector_search_loop:
    cmp word [bp + 2], ROOT_DIR_SECTOR_NUMBER
    jz load_error
    inc word [bp + 2]
    jmp sector_search_loop
    jmp hlt_loop

load_error:
    push 12
    push ERROR_MESSAGE
    call show_message
    add sp, 4

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

reset_floppy:
    push ax
    push dx
    xor ah, ah
    xor dl, dl
    int 13h
    pop dx
    pop ax
    ret

; read_sector(word sector_num, char* addr);
read_sector:
    push bp
    mov bp, sp

    mov ax, [bp + 6]
    mov es, ax          ; 目的内存地址段

    mov ax, [bp + 4]
    mov bx, 18
    div bx
    inc ax
    mov cl, al          ; 扇区

    mov bx, dx
    mov ax, dx
    and ax, 1
    mov dh, al          ; 磁头

    shr bx, 1
    mov ch, bl          ; 柱面

    mov bx, 0           ; 读取地址 es:bx

    mov dl, 0           ; 驱动器A
    mov ah, 0x02        ; 读盘指令
    mov al, 0x1         ; 读一个扇区
    int 0x13            ; 调用bios
    pop bp
    ret

hlt_loop:
    hlt
    jmp hlt_loop

BOOT_MESSAGE:
    db "booting..", 0
ERROR_MESSAGE:
    db "load error..", 0

times 510 - ($ - $$) db 0
dw 0xaa55