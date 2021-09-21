org 0x7c00

BOOT_STACK equ 0x7c00
ROOT_DIR_SECTOR_START_INDEX equ 19
ROOT_DIR_SECTOR_END_INDEX equ 22
FAT_ENTRY_ADDR equ 0x8000
FAT_ENTRY_ADDR_SEGMENT equ 0x800

disk_info:
    jmp boot_entry
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

boot_entry:
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

    ;push 4

    ;push LOADER_BIN
    ;push BOOT_MESSAGE
    ;call memcmp
    ;add sp, 6

    push FAT_ENTRY_ADDR_SEGMENT
    push 1
    call read_sector
    add sp, 4


    push 4
    call get_fat_entry
    add sp, 2

    call find_loader_entry

    jmp hlt_loop

load_error:
    push 12
    push ERROR_MESSAGE
    call show_message
    add sp, 4

    jmp hlt_loop

find_loader_entry:
    push bp
    mov bp, sp
    sub sp, 2

    ; [bp - 2] sector_start_index
    mov word [bp - 2], ROOT_DIR_SECTOR_START_INDEX

label_search_loop:
    cmp word [bp - 2], ROOT_DIR_SECTOR_END_INDEX
    jae label_not_find_loader

    push 0x820
    mov ax, [bp - 2]
    push ax
    call read_sector
    add sp, 2

    call find_sector_loader_entry
    add sp, 2

    inc word [bp - 2]
    jmp label_search_loop

label_not_find_loader:
    mov ax, 1

label_find_loader_final:
    mov sp, bp
    pop bp
    ret

find_sector_loader_entry:
    push bp
    mov bp, sp
    sub sp, 2

    ret

; memcmp(char* buffer1, char* buffer2, word size);
memcmp:
    push bp
    mov bp, sp
    push cx

    mov word [bp - 2], 0    ; word i
    jmp label_memcmp_check_loop

label_memcmp_loop:
    mov ax, [bp - 2]
    inc ax
    mov word [bp - 2], ax

label_memcmp_check_loop:
    mov cx, word [bp - 2]
    cmp cx, word [bp + 8]   ; size
    jae label_memcmp_true

    mov si, [bp + 4]        ; buffer1
    add si, [bp - 2]        ; buffer1 + i
    lodsb
    mov cl, al

    mov si, [bp + 6]        ; buffer2
    add si, [bp - 2]        ; buffer2 + i
    lodsb
    cmp al, cl
    je label_memcmp_loop

label_memcmp_false:
    mov ax, 1
    jmp label_memcmp_final
label_memcmp_true:
    xor ax, ax
label_memcmp_final:
    mov sp, bp
    pop bp
    ret

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

    mov sp, bp
    pop bp
    ret

; get_fat_entry(word index)
get_fat_entry:
    push bp
    mov bp, sp
    ; ret = index % 2
    mov ax, word [bp + 4]
    and ax, 1

    ; (fat_entry) + ((index - ret) * 3 / 2)
    mov dx, word [bp + 4]
    sub dx, ax
    imul dx, 3
    shr dx, 1
    add dx, FAT_ENTRY_ADDR

    ; ((ptr[1] & 0x0f) << 8) | ptr[0]
    cmp al, 1
    je label_index_odd_number
    mov si, dx
    inc si
    lodsb
    and ax, 0x0f
    shl ax, 8
    mov bx, ax

    mov si, dx
    lodsb
    or ax, bx
    jmp label_get_fat_entry_final

label_index_odd_number:
    ; (ptr[2] << 4) | ((ptr[1] >> 4) & 0x0F)
    mov si, dx
    add si, 2
    lodsb
    shl ax, 4
    mov bx, ax

    mov si, dx
    inc si
    lodsb
    shr ax, 4
    and ax, 0x0f
    or ax, bx

label_get_fat_entry_final:
    mov sp, bp
    pop bp
    ret

hlt_loop:
    hlt
    jmp hlt_loop

; 变量
g_loader_start_cluster dw 0
g_loader_file_size dw 0

; 字符串
BOOT_MESSAGE:
    db "booting..", 0
ERROR_MESSAGE:
    db "load error..", 0
LOADER_BIN:
    db "LOADER  BIN", 0

times 510 - ($ - $$) db 0
dw 0xaa55