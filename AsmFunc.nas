[FORMAT "WCOFF"]		;����Ŀ���ļ���ģʽ
[BITS 32]				;32λģʽ

[FILE "AamFunc.nas"]	;Դ�ļ���
	GLOBAL _io_hlt		;��������ĺ�����
	

[SECTION .text]			;Ŀ���ļ�д����Щ ֮����д����

_io_hlt:			;void io_hlt(void);
	HLT
	RET
	