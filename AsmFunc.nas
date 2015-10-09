[FORMAT "WCOFF"]		;����Ŀ���ļ���ģʽ
[INSTRSET "i486p"]		; ʹ�õ�486Ϊֹ��ָ��
[BITS 32]				;32λģʽ

[FILE "AamFunc.nas"]	;Դ�ļ���
	GLOBAL _io_hlt, _io_cli, _io_sti, _io_stihlt		;��������ĺ�����
	GLOBAL	_load_gdtr, _load_idtr
	
	GLOBAL	_asm_inthandler20,_asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
	
	GLOBAL	_io_in8,  _io_in16,  _io_in32
	GLOBAL	_io_out8, _io_out16, _io_out32
	
	GLOBAL	_io_load_eflags, _io_store_eflags
	GLOBAL	_load_cr0, _store_cr0,_load_tr
	
	GLOBAL	_memtest_sub,_farjmp
	
	EXTERN	_inthandler20,_inthandler21, _inthandler27, _inthandler2c
	
	
	
[SECTION .text]			;Ŀ���ļ�д����Щ ֮����д����
; ---------------------------------------------------------- 
_io_hlt:			;void io_hlt(void);
	HLT
	RET

_io_cli:	; void io_cli(void);	; �ر����п������ж�
	CLI
	RET

_io_sti:	; void io_sti(void);	; �����п������ж�
	STI
	RET

_io_stihlt:	; void io_stihlt(void);	
	STI
	HLT
	RET
		
; ---------------------------------------------------------- 
_load_gdtr:		; void load_gdtr(int limit, int addr);����GDT��limit��GDT�޳���addr��GDT��ַ
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LGDT	[ESP+6]
	RET

_load_idtr:		; void load_idtr(int limit, int addr);����IDT
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
	
_load_tr:		; void load_tr(int tr);		; ����TR�Ĵ�����ֵ
	LTR		[ESP+4]			; tr
	RET
	
; ---------------------------------------------------------- 
;	   ��PIC					��PIC
;	IRQ0	ʱ��			IRQ8	ʵʱ��	
;	IRQ1	����			IRQ9	INTOAH
;	IRQ2	����int			IRQ10	����
;	IRQ3	���п�2			IRQ11	����		
;	IEQ4	���п�1			IRQ12	PS2���
;	IEQ5	���п�2			IRQ13	Э������
;	IEQ6	����			IRQ14	Ӳ��	
;	IEQ7	���п�1			IRQ15	����	
; ----------------------------------------------------------
_asm_inthandler20:	; IRQ0	ʱ��
	PUSH	ES
	PUSH	DS
	PUSHAD					; ����Ĵ���
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; �޸�DS ES SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler20	; ���ô�����
	POP		EAX				; �ָ������Ĵ�����ֵ
	POPAD
	POP		DS
	POP		ES
	IRETD
		
_asm_inthandler21:	; IRQ 1		����
	PUSH	ES			
	PUSH	DS
	PUSHAD					; ����Ĵ���
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; �޸�DS ES SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler21	; ���ô�����
	POP		EAX				; �ָ������Ĵ�����ֵ
	POPAD
	POP		DS
	POP		ES
	IRETD					; �жϷ���

_asm_inthandler27:	; IRQ 7		���п�1
	PUSH	ES
	PUSH	DS
	PUSHAD					; ����Ĵ���
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; �޸�DS ES SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler27	; ���ô����� 
	POP		EAX				; �ָ������Ĵ�����ֵ
	POPAD
	POP		DS
	POP		ES
	IRETD					; �жϷ���

_asm_inthandler2c:	; IRQ 12	PS2���
	PUSH	ES
	PUSH	DS
	PUSHAD					; ����Ĵ���
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS			; �޸�DS ES SS
	MOV		DS,AX
	MOV		ES,AX			
	CALL	_inthandler2c	; ���ô�����
	POP		EAX				; �ָ������Ĵ�����ֵ
	POPAD
	POP		DS
	POP		ES
	IRETD					; �жϷ���
	
; ---------------------------------------------------------- 	
_io_in8:	; int io_in8(int port);	; ��port�˿ڶ���8λ���ݵ�AL��
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,0
	IN		AL,DX
	RET

_io_in16:	; int io_in16(int port); ; ��port�˿ڶ���16λ���ݵ�AX��
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,0
	IN		AX,DX
	RET

_io_in32:	; int io_in32(int port); ; ��port�˿ڶ���32λ���ݵ�EAX��
	MOV		EDX,[ESP+4]		; port
	IN		EAX,DX
	RET

; ---------------------------------------------------------- 		
_io_out8:	; void io_out8(int port, int data);		; ��8λ��data�����port�˿�
	MOV		EDX,[ESP+4]		; port
	MOV		AL,[ESP+8]		; data
	OUT		DX,AL
	RET

_io_out16:	; void io_out16(int port, int data);	; ��16λ��data�����port�˿�
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,[ESP+8]		; data
	OUT		DX,AX
	RET

_io_out32:	; void io_out32(int port, int data);	; ��32λ��data�����port�˿�
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,[ESP+8]		; data
	OUT		DX,EAX
	RET

; ---------------------------------------------------------- 		
_io_load_eflags:	; int io_load_eflags(void);		; ��EFLAGS�Ĵ��������ݷ���
	PUSHFD		; PUSH EFLAGS 
	POP		EAX	; EAX = EFLAGS
	RET

_io_store_eflags:	; void io_store_eflags(int eflags);	; ������eflags��������ΪEFLAGS�Ĵ�����ֵ
	MOV		EAX,[ESP+4]	
	PUSH	EAX
	POPFD		; POP EFLAGS 
	RET
	
; ---------------------------------------------------------- 		
; �ڴ��麯��	���start��ַ��ʼ��end��ַ�ķ�Χ��,�ܹ�ʹ�õ��ڴ��ĩβ��ַ,��������Ϊ����ֵ����
; ��memtest��������
_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
	PUSH	EDI						; ����EDI ESI EBX
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
_farjmp:		; void farjmp(int eip, int cs);		; Զ��ת
		JMP		FAR	[ESP+4]				; eip, cs
		RET