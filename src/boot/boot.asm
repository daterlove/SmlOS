org 0x7c00

BOOT_STACK equ 0x7c00
ROOT_DIR_SECTOR_START_INDEX equ 19
ROOT_DIR_SECTOR_END_INDEX equ 22
FAT_ENTRY_ADDR equ 0x8000
FAT_ENTRY_ADDR_SEGMENT equ 0x800
DATA_ADDR equ 0x9200
DATA_ADDR_SEGMENT equ 0x920

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

    int 13h         ; 重置软驱

    push FAT_ENTRY_ADDR_SEGMENT
    push 1
    call read_sector

    call find_loader_entry
    cmp ax, 0
    jne load_error

    call start_loader

start_loader:
    mov cx, DATA_ADDR_SEGMENT
    mov dx, [g_loader_start_cluster]

start_loader_loop:
    push cx
    add dx, 31              ; 定位到数据区位置
    push dx
    call read_sector
    add sp, 4

    sub dx, 31
    push dx
    call get_fat_entry      ; 获取下一个加载扇区
    add sp, 2

    cmp ax, 0xff7
    ja run_loader
    
    mov dx, ax
    add cx, 0x20            ; 加载段增加0x20即读取地址加512字节
    jmp start_loader_loop

run_loader:
    jmp DATA_ADDR           ; 跳转至loader

find_loader_entry:
    push bp
    mov bp, sp

    mov bx, ROOT_DIR_SECTOR_START_INDEX

label_search_loop:
    cmp bx, ROOT_DIR_SECTOR_END_INDEX
    jae label_find_loader_entry_false

    push DATA_ADDR_SEGMENT
    mov ax, bx
    push ax
    call read_sector
    add sp, 4

    push DATA_ADDR
    call find_sector_loader_entry
    add sp, 2
    cmp ax, 0
    jne label_find_loader_entry_true

    inc word [bp - 2]
    jmp label_search_loop

label_find_loader_entry_false:
    mov ax, 1
    jmp label_find_loader_entry_final

label_find_loader_entry_true:
    mov si, bx
    add si, 0x1a    ; 起始簇
    lodsw
    mov [g_loader_start_cluster], ax

    mov si, bx
    add si, 0x1c    ; 文件大小
    lodsw
    mov [g_loader_file_size], ax
    xor ax, ax

label_find_loader_entry_final:
    mov sp, bp
    pop bp
    ret

; find_sector_loader_entry(char* sector_addr)
; 返回值 对应项首地址
find_sector_loader_entry:
    push bp
    mov bp, sp
    mov dx, 0

find_sector_loader_loop:
    push 11
    push LOADER_BIN_STRING
    mov bx, [bp + 4]
    add bx, dx
    push bx
    call memcmp
    sub esp, 6
    cmp ax, 0
    je label_find_sector_loader_entry_true
    add dx, 32
    cmp dx, (512 - 32)
    ja label_find_sector_loader_entry_false
    jmp find_sector_loader_loop

label_find_sector_loader_entry_false:
    xor ax, ax
    jmp label_memcmp_final
label_find_sector_loader_entry_true:
    mov ax, bx
label_find_sector_loader_entry_final:
    mov sp, bp
    pop bp
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

; get_fat_entry(word index)
get_fat_entry:
    push bp
    mov bp, sp
    push bx
    push dx

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
    pop dx
    pop bx
    mov sp, bp
    pop bp
    ret

load_error:
    mov ax, 1301h
    mov bx, 000fh
    mov dx, 0000h
    mov cx, 12                  ; 字符串长度
    mov bp, ERROR_MESSAGE       ; es:bp字符串地址
    int 10h
    jmp hlt_loop

hlt_loop:
    hlt
    jmp hlt_loop

; 变量
g_loader_start_cluster dw 0
g_loader_file_size dw 0

; 字符串
ERROR_MESSAGE:
    db "load error..", 0
LOADER_BIN_STRING:
    db "LOADER  BIN", 0

times 510 - ($ - $$) db 0
dw 0xaa55