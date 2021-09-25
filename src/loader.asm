org 0x8200
jmp loader_entry

[SECTION .s16]
[BITS 16]

loader_entry:
    call clear_screen
    call reset_focus

    call open_a20_bus

    push 9
    push LOADING_MESSAGE
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
    mov sp, bp
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
    out	92h, al             ; 利用A20快速门开启总线
    pop	ax

    mov sp, bp
    pop bp
    ret

wait_keyboard:
    in al, 0x64             ; 从0x64端口读取数据
    and al, 0x2             ; 测试i8042输入缓冲区是否为空
    jnz wait_keyboard       ; 直到输入缓冲区为空为止
    ret

hlt_loop:
    hlt
    jmp hlt_loop

; 字符串
LOADING_MESSAGE:
    db "loading..", 0
