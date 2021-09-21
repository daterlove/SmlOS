org 0x8200

loader_entry:
    call clear_screen
    call reset_focus

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

; 字符串
LOADING_MESSAGE:
    db "loading..", 0
