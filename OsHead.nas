;---------------------------------------------
;�ļ�����OsHead.nas
;author:ǳ��˫��
;ʱ�䣺2015-9-19
;����������ϵͳ���� ��ǰ�ڴ������
;---------------------------------------------
[INSTRSET "i486p"]

VBEMODE	EQU		0x103			; 1024 x  76
BOTPAK	EQU		0x00280000		; �ں˴���λ�ַ
DSKCAC	EQU		0x00100000		; boot.bin��512B�����ƶ����õ�ַ�� (1M)
DSKCAC0	EQU		0x00008000		; ӳ���ļ� ���ص��ڴ� ����ʼ��ַ���ڶ�������

CYLS	EQU		0x0FF0
LEDS	EQU		0X0FF1
VMODE	EQU		0X0FF2
SCRNX	EQU		0X0FF4
SCRNY	EQU		0X0FF6
VRAM	EQU		0X0FF8
;---------------------------------------------
	ORG		0xc200
	
	;MOV AL,0X13
	;MOV	AH,0X00
	;INT	0X10
	
	;MOV	BYTE[VMODE],8	;8λ��ɫ
	;MOV	WORD[SCRNX],320
	;MOV WORD[SCRNY],200
	;MOV	DWORD[VRAM],0X000A0000	;VGA�Կ���320*200*8λ��ɫ���Դ��ַ
	
; ȷ��VBE�Ƿ����
	MOV		AX,0x9000		
	MOV		ES,AX
	MOV		DI,0			; ES:DI ���VBE��Ϣ
	MOV		AX,0x4f00		
	INT		0x10
	CMP		AX,0x004f		
	JNE		scrn320

; ���VBE�汾
	MOV		AX,[ES:DI+4]
	CMP		AX,0x0200
	JB		scrn320			; if (AX < 0x0200) goto scrn320

; ȡ�û���ģʽ��Ϣ
	MOV		CX,VBEMODE
	MOV		AX,0x4f01
	INT		0x10
	CMP		AX,0x004f
	JNE		scrn320

; ����ģʽ��Ϣȷ��
	CMP		BYTE [ES:DI+0x19],8
	JNE		scrn320
	CMP		BYTE [ES:DI+0x1b],4
	JNE		scrn320
	MOV		AX,[ES:DI+0x00]
	AND		AX,0x0080
	JZ		scrn320			; ģʽ���Ե�bit7��0  ���Է���

; ����ģʽ�л�

	MOV		BX,VBEMODE+0x4000
	MOV		AX,0x4f02
	INT		0x10
	MOV		BYTE [VMODE],8	; ��¼�»���ģʽ
	MOV		AX,[ES:DI+0x12]
	MOV		[SCRNX],AX
	MOV		AX,[ES:DI+0x14]
	MOV		[SCRNY],AX
	MOV		EAX,[ES:DI+0x28]
	MOV		[VRAM],EAX
	JMP		keystatus

scrn320:
	MOV		AL,0x13			; VGAͼ��320*200*8bit��ɫ
	MOV		AH,0x00
	INT		0x10
	MOV		BYTE [VMODE],8	; ��¼�»���ģʽ
	MOV		WORD [SCRNX],320
	MOV		WORD [SCRNY],200
	MOV		DWORD [VRAM],0x000a0000	

		
keystatus:	
	
	MOV	AH,0X02
	INT 0X16
	MOV	[LEDS],AL		;�������led��״̬
	
;---------------------------------------------	
;����ر������ж�
	MOV AL,0XFF
	OUT	0X21,AL			;������pic �ж�
	NOP					;����out���п��ܳ���
	OUT	0Xa1,AL			;���δ�pic �ն�
	
	CLI					;��ֹCPU�����ж�
	
;---------------------------------------------	
;��A20��ַ����,�Ա�CPU����1MB�����ڴ�
	CALL	waitkbdout	; ���i8042�����뻺����
	MOV		AL,0XD1
	OUT		0X64,AL
	CALL	waitkbdout
	
	MOV		AL,0xdf			;����A20����
	OUT		0X60,AL
	CALL	waitkbdout
	
	
;---------------------------------------------		
[INSTRSET "i486p"]			;ʹ��486ָ�
	LGDT	[GDTR0]			; ����GDTR�Ĵ�������ʱ����
	MOV		EAX,CR0
	AND		EAX,0X7fffffff	;��ֹ��ҳģʽ
	OR		EAX,0X00000001	;��������ģʽ
	MOV		CR0,EAX
	
	JMP		protectMode		;�л�������ģʽ��ָ����ͱ仯������������ת
	
;---------------------------------------------	
;���뱣��ģʽ	
protectMode:
	MOV		AX,1*8			; �������öμĴ���
	MOV		DS,AX			; DS, ES, FS, GS, SS��ָ��GDT�е��ں����ݶ�������
	MOV		ES,AX
	MOV		FS,AX
	MOV		GS,AX
	MOV		SS,AX
	
;----------------------------------------------
; �����ļ�֮���512KB����(�ں˴���)���Ƶ�0x00280000��

; ��DS:ESIָ���512KB�����ݸ��Ƶ�ES:EDIָ����ڴ浥Ԫ
; DS:ESI = 8:���ļ�����λ��
; ES:EDI = 8:0x00280000 (�ں˴���εĻ���ַ��0x00280000)

	MOV		ESI,bootpack	;Դ��ַ
	MOV		EDI,BOTPAK		;Ŀ�ĵ�ַ
	MOV		ECX,512*1024/4	;���ƵĴ�С��512K
	CALL	memcpy	
;----------------------------------------------
;�����������Ĵ��븴�Ƶ�0x00100000�� (1M)
	MOV		ESI,0x7c00		
	MOV		EDI,DSKCAC		
	MOV		ECX,512/4
	CALL	memcpy

;----------------------------------------------
;����ӳ���ж�������ݴ�0x8200�����Ƶ�0x00100200��
	MOV		ESI,DSKCAC0+512	; ESI = 0x8200
	MOV		EDI,DSKCAC+512	; EDI = 0x00100200
	MOV		ECX,0
	MOV		CL,BYTE [CYLS]	; CL = �������������
	IMUL	ECX,512*18*2/4	; һ��Ҫ���ƵĴ���
	SUB		ECX,512/4		; ��ȥ(�����������ֽ���/4)
	CALL	memcpy
		
;----------------------------------------------
;�ں˵�����
	MOV		EBX,BOTPAK		; EBX = 0x00280000
	MOV		ECX,[EBX+16]	; ECX = 0x11a8
	ADD		ECX,3			; ECX += 3;
	SHR		ECX,2			; ECX /= 4;
	JZ		skip			; û��Ҫ���͵�����ʱ
	
	MOV		ESI,[EBX+20]	; ���Ƶ�Դ��ַ(�����bootoack.hrbͷ����ƫ��) ESI = 0x10c8
	ADD		ESI,EBX			; �����Ҫ���Ƶ����ݵ�ʵ�������ַ
	MOV		EDI,[EBX+12]	; ���Ƶ�Ŀ�ĵ�ַ	EDI = 0x310000
	CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; ջ��ʼֵ	ESP = 0x310000
		JMP		DWORD 2*8:0x0000001b
		
fin:
	HLT
	JMP		fin
;---------------------------------------------	
waitkbdout:						
	IN	AL,0x64			; ��0x64�˿ڶ�ȡ����
	AND	AL,0x02			; ����i8042���뻺�����Ƿ�Ϊ��
	JNZ	waitkbdout		; ����Ϊ�� �������ȡ ֱ�����뻺����Ϊ��Ϊֹ
	RET					; Ϊ�� �򷵻�	
	
;----------------------------------------------------------------------	
; �ú�����DS:ESIָ����ڴ浥Ԫ�����ݸ��Ƶ�ES:EDIָ����ڴ浥Ԫ
; ÿ�θ���4���ֽ�
memcpy:
	MOV		EAX,[ESI]		
	ADD		ESI,4
	MOV		[EDI],EAX
	ADD		EDI,4
	SUB		ECX,1
	JNZ		memcpy			
	RET	
	
;---------------------------------------------		
GDT0:
	RESB	8						;��������
	DW	0xffff,0x0000,0x9200,0x00cf	;32λ�ɶ�д���ݶ�������	���޳� 4G-1���λ�ַΪ0  	
					 				;Ҳ����˵�öο�Ѱַ0~4G-1	DPL = 0 �ں����ݶ�
	DW	0xffff,0x0000,0x9a28,0x0047	;32λ�ɶ���ִ�д����������	���޳�512KB���λ�ַΪ0x280000
									;DPL = 0	�ں˴����

	DW		0
	
GDTR0:										
	DW	8*3-1						; GDT�޳�
	DD	GDT0						; GDT��ַ
	ALIGNB	16
	
bootpack:
;�������ں�	
