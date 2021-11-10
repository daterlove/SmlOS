org 0x9400

BOOT_STACK                  equ 0x7c00

ROOT_diR_SECTOR_START_INDEX equ 19
ROOT_diR_SECTOR_END_INDEX   equ 22

FAT_ENTRY_ADDR              equ 0x8000
FAT_ENTRY_ADDR_SEGMENT      equ 0x800

DATA_ADDR                   equ 0x9100
DATA_ADDR_SEGMENT           equ 0x910

VBE_INFO_ADDR               equ 0x9000
MEMORY_INFO_ADDR            equ 0x7e00

KERNEL_ADDR                 equ 0x100000

VBEMODE                     equ  0x180
CYLS                        equ  0x0FF0
LEDS                        equ  0X0FF1
VMODE                       equ  0X0FF2
SCREEN_X                    equ  0X0FF4
SCREEN_Y                    equ  0X0FF6
VRAM                        equ  0X0FF8

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

    push 9
    push LOADING_MESSAGE
    call show_message
    add sp, 4

    cli
    call open_a20_bus

    lgdt [GDTR32_TEMP]

    mov eax, cr0
    or eax, 1
    mov cr0, eax                ; 开启保护模式

    mov ax, SELECTOR_DATA32     ; 设置段寄存器, 进入big real模式
    mov fs, ax

    mov eax, cr0
    and al, 11111110b
    mov cr0, eax                ; 返回实模式

    sti

    call reset_floppy           ; 重置软驱

    call load_fat_entry

    call find_kernel_entry
    cmp ax, 0
    jne load_kernel_error

    call read_kernel_data       ; 加载内核

    call get_memory_info

    call open_vbe

    cli
    lgdt [GDTR32_TEMP]

    mov eax, cr0
    or eax, 1
    mov cr0, eax                ; 开启保护模式

    jmp dword SELECTOR_CODE32:protect_mode            ; 切换到保护模式，指令解释变化，必须立马跳转

get_memory_info:
    push bp
    mov bp, sp
    push eax
    push ebx
    push ecx
    push edx

    mov ebx, 0
    mov ax, 0
    mov es, ax
    mov di, MEMORY_INFO_ADDR

get_memory_info_loop:
    mov eax, 0x0e820
    mov ecx, 20
    mov edx, 0x534d4150
    int 0x15
    add di, 20
    cmp ebx, 0
    jne get_memory_info_loop

get_memory_info_final:
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov sp, bp
    pop bp
    ret

open_vbe:
    push bp
    mov bp, sp
    push eax
    push bx
    push cx
    push dx
    push es
    push di
    push si

    ; 获取vbe状态
    mov ax, 0x00
    mov es, ax
    mov di, VBE_INFO_ADDR
    mov ax, 0x4f00
    int 0x10
    cmp ax, 0x004f
    jne label_open_vbe_error

    ; 判断vbe版本
    mov ax, [es:di + 4]
    cmp ax, 0x0200
    jb label_open_vbe_error

    ; 获取画面模式信息
    mov cx, VBEMODE
    mov ax, 0x4f01
    int 0x10
    cmp ax, 0x004f
    jne label_open_vbe_error

    ; 切换vbe
    mov bx, VBEMODE + 0x4000
    mov ax, 0x4f02
    int 0x10
    cmp ax, 0x004f
    jne label_open_vbe_error

    ; 记录信息
    mov byte [VMODE], 32
    mov ax, [es:di + 0x12]
    mov [SCREEN_X], ax
    mov ax, [es:di + 0x14]
    mov [SCREEN_Y], ax
    mov eax, [es:di + 0x28]
    mov [VRAM], eax

    jmp label_open_vbe_final

label_open_vbe_error:
    push 20
    push GET_VBE_ERROR_MESSAGE
    call show_message
    add sp, 4
    jmp hlt_loop

label_open_vbe_final:
    pop si
    pop di
    pop es
    pop dx
    pop cx
    pop bx
    pop eax
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
    mov cx, [bp + 6]            ; 字符串长度
    mov bp, [bp + 4]            ; es:bp字符串地址
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
    mov es, ax                  ; 目的内存地址段

    mov ax, [bp + 4]
    mov bl, 18
    div bl

    mov bl, ah
    inc bl
    mov cl, bl                  ; 扇区

    mov bl, al
    and bl, 1
    mov dh, bl                  ; 磁头

    mov bl, al
    shr bl, 1
    mov ch, bl                  ; 柱面

    mov bx, 0                   ; 读取地址 es:bx
    mov dl, 0                   ; 驱动器A

label_reading_retry:
    mov ah, 0x02                ; 读盘指令
    mov al, 0x1                 ; 读一个扇区
    int 0x13                    ; 调用bios
    jc label_reading_retry

    pop dx
    pop cx
    pop bx
    mov sp, bp
    pop bp
    ret

read_kernel_data:
    push bp
    mov bp, sp
    push bx
    push ecx
    push dx

    mov ecx, KERNEL_ADDR
    mov dx, [g_kernel_start_cluster]

read_kernel_data_loop:
    push DATA_ADDR_SEGMENT
    add dx, 31              ; 定位到数据区位置
    push dx
    call read_sector
    add sp, 4

    mov eax, 512
    push eax
    push ecx
    mov eax, DATA_ADDR
    push eax
    call memcpy
    add sp, 12

    sub dx, 31
    push dx
    call get_fat_entry      ; 获取下一个加载扇区
    add sp, 2

    cmp ax, 0xff7
    ja read_kernel_data_loop_final

    mov dx, ax
    add ecx, 512            ; 地址增加512字节
    jmp read_kernel_data_loop

read_kernel_data_loop_final:
    pop dx
    pop ecx
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

find_kernel_entry:
    push bp
    mov bp, sp
    push bx

    mov bx, ROOT_diR_SECTOR_START_INDEX

label_search_loop:
    cmp bx, ROOT_diR_SECTOR_END_INDEX
    jae label_find_kernel_entry_false

    push DATA_ADDR_SEGMENT
    mov ax, bx
    push ax
    call read_sector
    add sp, 4

    push DATA_ADDR
    call find_sector_kernel_entry
    add sp, 2
    cmp ax, 0
    jne label_find_kernel_entry_true

    inc word [bp - 2]
    jmp label_search_loop

label_find_kernel_entry_false:
    mov ax, 1
    jmp label_find_kernel_entry_final

label_find_kernel_entry_true:
    mov bx, ax
    mov si, bx
    add si, 0x1a            ; 起始簇
    lodsw
    mov [g_kernel_start_cluster], ax

    mov si, bx
    add si, 0x1c            ; 文件大小
    lodsw
    mov [g_kernel_file_size], ax
    xor ax, ax

label_find_kernel_entry_final:
    pop bx
    mov sp, bp
    pop bp
    ret

; find_sector_kernel_entry(char* sector_addr)
; 返回值 对应项首地址
find_sector_kernel_entry:
    push bp
    mov bp, sp
    push bx
    push dx

    mov dx, 0

find_sector_kernel_loop:
    push 11
    push KERNEL_BIN_STRING
    mov bx, [bp + 4]
    add bx, dx
    push bx
    call memcmp
    sub esp, 6
    cmp ax, 0
    je label_find_sector_kernel_entry_true
    add dx, 32
    cmp dx, (512 - 32)
    ja label_find_sector_kernel_entry_false
    jmp find_sector_kernel_loop

label_find_sector_kernel_entry_false:
    xor ax, ax
    jmp label_memcmp_final
label_find_sector_kernel_entry_true:
    mov ax, bx
label_find_sector_kernel_entry_final:
    pop dx
    pop bx
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

; memcpy(char* src, char* dest, int size);
memcpy:
    push bp
    mov bp, sp
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi

    mov esi, [bp + 4]
    mov edi, [bp + 8]
    mov ebx, [bp + 12]
    mov ecx, 0

label_memcpy_loop:
    mov dl, byte [ds:esi]
    mov byte [fs:edi], dl
    inc esi
    inc edi
    inc ecx
    cmp ecx, ebx
    jb label_memcpy_loop

    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov sp, bp
    pop bp
    ret

load_kernel_error:
    push 19
    push KERNEL_LOAD_ERROR_MESSAGE
    call show_message
    add sp, 4

    jmp hlt_loop

hlt_loop:
    hlt
    jmp hlt_loop

; 变量
g_kernel_start_cluster dw 0
g_kernel_file_size dw 0

; 字符串
LOADING_MESSAGE:
    db "loading..", 0
KERNEL_LOAD_ERROR_MESSAGE:
    db "kernel load error..", 0
GET_VBE_ERROR_MESSAGE:
    db "get vbe info error..", 0
KERNEL_BIN_STRING:
    db "KERNEL  BIN", 0

GDT32_TEMP:
    dw 0x0000, 0x0000, 0x0000, 0x0000   ; 空描述符
LABEL_DesC_DATA32:
    dw 0xffff, 0x0000, 0x9200, 0x00cf   ; 可读写数据段
LABEL_DesC_CODE32:
    dw 0xffff, 0x0000, 0x9a00, 0x00cf   ; 可读可执行代码段

SELECTOR_DATA32 equ LABEL_DesC_DATA32 - GDT32_TEMP
SELECTOR_CODE32 equ LABEL_DesC_CODE32 - GDT32_TEMP

GDTR32_TEMP:
    dw 8 * 3 - 1                ; GDT限长
    dd GDT32_TEMP               ; GDT基址

GDT64_TEMP:
    dq 0x0000000000000000
LABEL_DesC_DATA64:
    dq 0x0000920000000000
LABEL_DesC_CODE64:
    dq 0x0020980000000000

SELECTOR_DATA64 equ LABEL_DesC_DATA64 - GDT64_TEMP
SELECTOR_CODE64 equ LABEL_DesC_CODE64 - GDT64_TEMP

GDTR64_TEMP:
    dw 8 * 3 - 1                ; GDT限长
    dd GDT64_TEMP               ; GDT基址

[SECTION .s32]
[BITS 32]

;进入保护模式
protect_mode:
    mov ax, SELECTOR_DATA32     ; 重新设置段寄存器
    mov ds, ax                  ; DS, es, FS, GS, SS都指向GDT中的内核数据段描述符
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, BOOT_STACK

    ; 初始化临时页表
    mov dword [0x90000], 0x91007
    mov dword [0x90800], 0x91007
    mov dword [0x91000], 0x92007
    mov dword [0x92000], 0x000083
    mov dword [0x92008], 0x200083
    mov dword [0x92010], 0x400083
    mov dword [0x92018], 0x600083
    mov dword [0x92020], 0x800083
    mov dword [0x92028], 0xa00083

    ; 加载临时gdtr
    cli
    lgdt [GDTR64_TEMP]
    mov ax, SELECTOR_DATA64
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, BOOT_STACK

    ; 开启pae
    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    ; 加载cr3
    mov eax, 0x90000
    mov cr3, eax

    ;进入long-mode
    mov ecx, 0C0000080h  ;IA32_EFER
    rdmsr

    bts eax, 8
    wrmsr

    ; 开启分页
    mov eax, cr0
    bts eax, 0
    bts eax, 31
    mov cr0, eax

    ; 跳转到kernel
    jmp SELECTOR_CODE64:KERNEL_ADDR

