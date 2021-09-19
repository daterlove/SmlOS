org 0x7c00

BOOT_STACK equ 0x7c00

disk_info:
    jmp entry
    nop
    db "SML-OS  "       ; 启动区的名称,8字符
    dw 512              ; 每扇区字节数
    db  1               ; 每簇扇区数
    dw  1               ; FAT文件分配表之前的引导扇区
    db  2               ; FAT表的个数
    dw  224             ; 根目录 文件最大数
    dw  2880            ; 逻辑 扇区 总数
    db  0xf0            ; 磁盘种类
    dw  9               ; 每个FAT占用 多少扇区
    dw  18              ; 每磁道 扇区数
    dw  2               ; 磁头数
    dd  0               ; 隐藏扇区数
    dd  2880            ; 如果逻辑扇区总数为0，则在这里记录扇区总数
    db  0,0,0x29        ; 中断13的驱动器号，未使用，扩展引导标志
    dd  0Xffffffff      ; 卷序列号
    db  "SML-OS     "   ; 卷标, 11个字符
    db  "FAT12   "      ; 文件系统类型，8个字符

entry:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, BOOT_STACK
    mov bp, sp

    call clear_screen
    call reset_focus
    push BOOT_MESSAGE
    call show_message
    pop ax
    jmp hlt_loop

show_message:
    push bp
    push ax
    push bx
    push dx
    push cx
    push es
    mov	ax,	ds
    mov	es,	ax
    mov ax, 1301h
    mov bx, 000fh
    mov dx, 0000h
    mov cx, 10
    mov bp, [bp - 2]
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

hlt_loop:
    hlt
    jmp hlt_loop

BOOT_MESSAGE:
    db "booting.."

times 510 - ($ - $$) db 0
dw 0xaa55