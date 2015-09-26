;---------------------------------------------
;文件名：OsHead.nas
;author:浅握双手
;时间：2015-9-19
;功能描述：系统启动 的前期处理程序
;---------------------------------------------
[INSTRSET "i486p"]

VBEMODE	EQU		0x103			; 1024 x  76
BOTPAK	EQU		0x00280000		; 内核代码段基址
DSKCAC	EQU		0x00100000		; boot.bin的512B将被移动到该地址处 (1M)
DSKCAC0	EQU		0x00008000		; 映像文件 加载到内存 的起始地址（第二扇区）

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
	
	;MOV	BYTE[VMODE],8	;8位彩色
	;MOV	WORD[SCRNX],320
	;MOV WORD[SCRNY],200
	;MOV	DWORD[VRAM],0X000A0000	;VGA显卡，320*200*8位彩色，显存地址
	
; 确认VBE是否存在
	MOV		AX,0x9000		
	MOV		ES,AX
	MOV		DI,0			; ES:DI 存放VBE信息
	MOV		AX,0x4f00		
	INT		0x10
	CMP		AX,0x004f		
	JNE		scrn320

; 检测VBE版本
	MOV		AX,[ES:DI+4]
	CMP		AX,0x0200
	JB		scrn320			; if (AX < 0x0200) goto scrn320

; 取得画面模式信息
	MOV		CX,VBEMODE
	MOV		AX,0x4f01
	INT		0x10
	CMP		AX,0x004f
	JNE		scrn320

; 画面模式信息确认
	CMP		BYTE [ES:DI+0x19],8
	JNE		scrn320
	CMP		BYTE [ES:DI+0x1b],4
	JNE		scrn320
	MOV		AX,[ES:DI+0x00]
	AND		AX,0x0080
	JZ		scrn320			; 模式属性的bit7是0  所以放弃

; 画面模式切换

	MOV		BX,VBEMODE+0x4000
	MOV		AX,0x4f02
	INT		0x10
	MOV		BYTE [VMODE],8	; 记录下画面模式
	MOV		AX,[ES:DI+0x12]
	MOV		[SCRNX],AX
	MOV		AX,[ES:DI+0x14]
	MOV		[SCRNY],AX
	MOV		EAX,[ES:DI+0x28]
	MOV		[VRAM],EAX
	JMP		keystatus

scrn320:
	MOV		AL,0x13			; VGA图、320*200*8bit彩色
	MOV		AH,0x00
	INT		0x10
	MOV		BYTE [VMODE],8	; 记录下画面模式
	MOV		WORD [SCRNX],320
	MOV		WORD [SCRNY],200
	MOV		DWORD [VRAM],0x000a0000	

		
keystatus:	
	
	MOV	AH,0X02
	INT 0X16
	MOV	[LEDS],AL		;保存键盘led灯状态
	
;---------------------------------------------	
;这里关闭所有中断
	MOV AL,0XFF
	OUT	0X21,AL			;屏蔽主pic 中断
	NOP					;连续out，有可能出错
	OUT	0Xa1,AL			;屏蔽从pic 终端
	
	CLI					;禁止CPU级别中断
	
;---------------------------------------------	
;打开A20地址总线,以便CPU访问1MB以上内存
	CALL	waitkbdout	; 清空i8042的输入缓冲区
	MOV		AL,0XD1
	OUT		0X64,AL
	CALL	waitkbdout
	
	MOV		AL,0xdf			;开启A20总线
	OUT		0X60,AL
	CALL	waitkbdout
	
	
;---------------------------------------------		
[INSTRSET "i486p"]			;使用486指令集
	LGDT	[GDTR0]			; 加载GDTR寄存器，临时设置
	MOV		EAX,CR0
	AND		EAX,0X7fffffff	;禁止分页模式
	OR		EAX,0X00000001	;开启保护模式
	MOV		CR0,EAX
	
	JMP		protectMode		;切换到保护模式，指令解释变化，必须立马跳转
	
;---------------------------------------------	
;进入保护模式	
protectMode:
	MOV		AX,1*8			; 重新设置段寄存器
	MOV		DS,AX			; DS, ES, FS, GS, SS都指向GDT中的内核数据段描述符
	MOV		ES,AX
	MOV		FS,AX
	MOV		GS,AX
	MOV		SS,AX
	
;----------------------------------------------
; 将该文件之后的512KB内容(内核代码)复制到0x00280000处

; 将DS:ESI指向的512KB的内容复制到ES:EDI指向的内存单元
; DS:ESI = 8:该文件随后的位置
; ES:EDI = 8:0x00280000 (内核代码段的基地址是0x00280000)

	MOV		ESI,bootpack	;源地址
	MOV		EDI,BOTPAK		;目的地址
	MOV		ECX,512*1024/4	;复制的大小，512K
	CALL	memcpy	
;----------------------------------------------
;将启动扇区的代码复制到0x00100000处 (1M)
	MOV		ESI,0x7c00		
	MOV		EDI,DSKCAC		
	MOV		ECX,512/4
	CALL	memcpy

;----------------------------------------------
;将从映像中读入的数据从0x8200处复制到0x00100200处
	MOV		ESI,DSKCAC0+512	; ESI = 0x8200
	MOV		EDI,DSKCAC+512	; EDI = 0x00100200
	MOV		ECX,0
	MOV		CL,BYTE [CYLS]	; CL = 共读入的柱面数
	IMUL	ECX,512*18*2/4	; 一共要复制的次数
	SUB		ECX,512/4		; 减去(启动扇区的字节数/4)
	CALL	memcpy
		
;----------------------------------------------
;内核的启动
	MOV		EBX,BOTPAK		; EBX = 0x00280000
	MOV		ECX,[EBX+16]	; ECX = 0x11a8
	ADD		ECX,3			; ECX += 3;
	SHR		ECX,2			; ECX /= 4;
	JZ		skip			; 没有要传送的数据时
	
	MOV		ESI,[EBX+20]	; 复制的源地址(相对于bootoack.hrb头部的偏移) ESI = 0x10c8
	ADD		ESI,EBX			; 换算出要复制的数据的实际物理地址
	MOV		EDI,[EBX+12]	; 复制的目的地址	EDI = 0x310000
	CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 栈初始值	ESP = 0x310000
		JMP		DWORD 2*8:0x0000001b
		
fin:
	HLT
	JMP		fin
;---------------------------------------------	
waitkbdout:						
	IN	AL,0x64			; 从0x64端口读取数据
	AND	AL,0x02			; 测试i8042输入缓冲区是否为空
	JNZ	waitkbdout		; 若不为空 则继续读取 直到输入缓冲区为空为止
	RET					; 为空 则返回	
	
;----------------------------------------------------------------------	
; 该函数将DS:ESI指向的内存单元的内容复制到ES:EDI指向的内存单元
; 每次复制4个字节
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
	RESB	8						;空描述符
	DW	0xffff,0x0000,0x9200,0x00cf	;32位可读写数据段描述符	段限长 4G-1，段基址为0  	
					 				;也就是说该段可寻址0~4G-1	DPL = 0 内核数据段
	DW	0xffff,0x0000,0x9a28,0x0047	;32位可读可执行代码段描述符	段限长512KB，段基址为0x280000
									;DPL = 0	内核代码段

	DW		0
	
GDTR0:										
	DW	8*3-1						; GDT限长
	DD	GDT0						; GDT基址
	ALIGNB	16
	
bootpack:
;后面是内核	
