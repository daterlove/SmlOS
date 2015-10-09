[FORMAT "WCOFF"]		;制作目标文件的模式
[INSTRSET "i486p"]		; 使用到486为止的指令
[BITS 32]				;32位模式

[FILE "AamFunc.nas"]	;源文件名
	GLOBAL _io_hlt, _io_cli, _io_sti, _io_stihlt		;程序包含的函数名
	GLOBAL	_load_gdtr, _load_idtr
	
	GLOBAL	_asm_inthandler20,_asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
	
	GLOBAL	_io_in8,  _io_in16,  _io_in32
	GLOBAL	_io_out8, _io_out16, _io_out32
	
	GLOBAL	_io_load_eflags, _io_store_eflags
	GLOBAL	_load_cr0, _store_cr0,_load_tr
	
	GLOBAL	_memtest_sub,_farjmp
	
	EXTERN	_inthandler20,_inthandler21, _inthandler27, _inthandler2c
	
	
	
[SECTION .text]			;目标文件写了这些 之后再写函数
; ---------------------------------------------------------- 
_io_hlt:			;void io_hlt(void);
	HLT
	RET

_io_cli:	; void io_cli(void);	; 关闭所有可屏蔽中断
	CLI
	RET

_io_sti:	; void io_sti(void);	; 打开所有可屏蔽中断
	STI
	RET

_io_stihlt:	; void io_stihlt(void);	
	STI
	HLT
	RET
		
; ---------------------------------------------------------- 
_load_gdtr:		; void load_gdtr(int limit, int addr);加载GDT，limit是GDT限长，addr是GDT基址
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LGDT	[ESP+6]
	RET

_load_idtr:		; void load_idtr(int limit, int addr);加载IDT
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LIDT	[ESP+6]
	RET
	
_load_cr0:		; int load_cr0(void);
	MOV		EAX,CR0
	RET

_store_cr0:		; void store_cr0(int cr0);
	MOV		EAX,[ESP+4]
	MOV		CR0,EAX
	RET		
	
_load_tr:		; void load_tr(int tr);		; 设置TR寄存器的值
	LTR		[ESP+4]			; tr
	RET
	
; ---------------------------------------------------------- 
;	   主PIC					从PIC
;	IRQ0	时钟			IRQ8	实时钟	
;	IRQ1	键盘			IRQ9	INTOAH
;	IRQ2	接连int			IRQ10	保留
;	IRQ3	串行口2			IRQ11	保留		
;	IEQ4	串行口1			IRQ12	PS2鼠标
;	IEQ5	并行口2			IRQ13	协处理器
;	IEQ6	软盘			IRQ14	硬盘	
;	IEQ7	并行口1			IRQ15	保留	
; ----------------------------------------------------------
_asm_inthandler20:	; IRQ0	时钟
	PUSH	ES
	PUSH	DS
	PUSHAD					; 保存寄存器
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; 修改DS ES SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler20	; 调用处理函数
	POP		EAX				; 恢复各个寄存器的值
	POPAD
	POP		DS
	POP		ES
	IRETD
		
_asm_inthandler21:	; IRQ 1		键盘
	PUSH	ES			
	PUSH	DS
	PUSHAD					; 保存寄存器
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; 修改DS ES SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler21	; 调用处理函数
	POP		EAX				; 恢复各个寄存器的值
	POPAD
	POP		DS
	POP		ES
	IRETD					; 中断返回

_asm_inthandler27:	; IRQ 7		并行口1
	PUSH	ES
	PUSH	DS
	PUSHAD					; 保存寄存器
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; 修改DS ES SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler27	; 调用处理函数 
	POP		EAX				; 恢复各个寄存器的值
	POPAD
	POP		DS
	POP		ES
	IRETD					; 中断返回

_asm_inthandler2c:	; IRQ 12	PS2鼠标
	PUSH	ES
	PUSH	DS
	PUSHAD					; 保存寄存器
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; 修改DS ES SS
	MOV		DS,AX
	MOV		ES,AX			
	CALL	_inthandler2c	; 调用处理函数
	POP		EAX				; 恢复各个寄存器的值
	POPAD
	POP		DS
	POP		ES
	IRETD					; 中断返回
	
; ---------------------------------------------------------- 	
_io_in8:	; int io_in8(int port);	; 从port端口读入8位数据到AL中
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,0
	IN		AL,DX
	RET

_io_in16:	; int io_in16(int port); ; 从port端口读入16位数据到AX中
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,0
	IN		AX,DX
	RET

_io_in32:	; int io_in32(int port); ; 从port端口读入32位数据到EAX中
	MOV		EDX,[ESP+4]		; port
	IN		EAX,DX
	RET

; ---------------------------------------------------------- 		
_io_out8:	; void io_out8(int port, int data);		; 将8位的data输出到port端口
	MOV		EDX,[ESP+4]		; port
	MOV		AL,[ESP+8]		; data
	OUT		DX,AL
	RET

_io_out16:	; void io_out16(int port, int data);	; 将16位的data输出到port端口
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,[ESP+8]		; data
	OUT		DX,AX
	RET

_io_out32:	; void io_out32(int port, int data);	; 将32位的data输出到port端口
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,[ESP+8]		; data
	OUT		DX,EAX
	RET

; ---------------------------------------------------------- 		
_io_load_eflags:	; int io_load_eflags(void);		; 将EFLAGS寄存器的内容返回
	PUSHFD		; PUSH EFLAGS 
	POP		EAX	; EAX = EFLAGS
	RET

_io_store_eflags:	; void io_store_eflags(int eflags);	; 将参数eflags的内容作为EFLAGS寄存器的值
	MOV		EAX,[ESP+4]	
	PUSH	EAX
	POPFD		; POP EFLAGS 
	RET
	
; ---------------------------------------------------------- 		
; 内存检查函数	检查start地址开始到end地址的范围内,能够使用的内存的末尾地址,并将其作为返回值返回
; 由memtest函数调用
_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
	PUSH	EDI						; 保存EDI ESI EBX
	PUSH	ESI
	PUSH	EBX
	MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
	MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
	MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
	MOV		EBX,EAX
	ADD		EBX,0xffc				; p = i + 0xffc;
	MOV		EDX,[EBX]				; old = *p;
	MOV		[EBX],ESI				; *p = pat0;
	XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
	CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
	JNE		mts_fin
	XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
	CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
	JNE		mts_fin
	MOV		[EBX],EDX				; *p = old;
	ADD		EAX,0x1000				; i += 0x1000;
	CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
	JBE		mts_loop
	POP		EBX
	POP		ESI
	POP		EDI
	RET
mts_fin:
	MOV		[EBX],EDX				; *p = old;
	POP		EBX
	POP		ESI
	POP		EDI
	RET
	
; ---------------------------------------------------------- 	
_farjmp:		; void farjmp(int eip, int cs);		; 远跳转
		JMP		FAR	[ESP+4]				; eip, cs
		RET