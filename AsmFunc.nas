[FORMAT "WCOFF"]		;制作目标文件的模式
[BITS 32]				;32位模式

[FILE "AamFunc.nas"]	;源文件名
	GLOBAL _io_hlt		;程序包含的函数名
	

[SECTION .text]			;目标文件写了这些 之后再写函数

_io_hlt:			;void io_hlt(void);
	HLT
	RET
	