;----------------------------------
;文件名：boot.nas
;author:浅握双手
;时间：2015-9-16
;功能描述：系统启动 的引导程序
;----------------------------------

	ORG		0x7c00		;启动程序装载地址

	JMP		entry		;跳转到程序入口
	DB		0x90		;NOP 空指令，

	DB		"SML-OS  "	;启动区的名称,8字符
	DW		512			;每扇区字节数
	DB		1			;每簇扇区数
	DW		1			;代表FAT文件分配表之前的引导扇区

	DB		2			;FAT表的个数
	DW		224			;根目录 文件最大数
	DW		2880		;逻辑 扇区 总数
	DB		0xf0		;磁盘种类
	DW		9			;每个FAT占用 多少扇区
	DW		18			;每磁道 扇区数
	DW		2			;磁头数
	DD		0			;隐藏扇区数

	DD		2880		;如果逻辑扇区总数为0，则在这里记录扇区总数
	DB		0,0,0x29	;中断13的驱动器号，未使用，扩展引导标志
	DD		0Xffffffff	;卷序列号
	DB		"SML-OS     "	;卷标，必须11个字符
	DB		"FAT12   "	; 文件系统类型，必须是8个字符，不足填充空格
	RESB	18

;程序主体入口
entry:
	MOV		AX,0		;初始化寄存器
	MOV		SS,AX
	MOV 	SP,0x7C00
	MOV		DS,AX
	MOV		ES,AX
	
	MOV 	SI,msg

putloop:
	MOV		AL,[SI]
	ADD		Si,1
	CMP		AL,0
	JE		fin
	MOV		AH,0x0e
	MOV 	BX,15
	INT 	0x10
	
	JMP		putloop

fin:
	HLT
	JMP 	fin
	
msg:
	DB		0x0a, 0x0a		; 换行
	DB		"Just My Sml_OS"
	DB		0x0a			; 换行
	DB		0

	;----------------------------------
	RESB	0x7dfe-$		; 填充字符
	DB		0x55, 0xaa			;最后两个字节代表启动扇区










