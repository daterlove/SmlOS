#include "common.h"


/* ��ʼ��PIC */
void Init_PIC(void)
{
	io_out8(PIC0_IMR,  0xff  ); /* ��ֹ��PIC�����ж� */
	io_out8(PIC1_IMR,  0xff  ); /* ��ֹ��PIC�����ж� */

	io_out8(PIC0_ICW1, 0x11  ); /* ��ҪICW4�� ��Ƭ������ ���ش�����ʽ */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7����INT 0x20~0x27���� */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1��IRQ2���� */
	io_out8(PIC0_ICW4, 0x01  ); /* ��ͨȫǶ�� �ǻ��� ���Զ������жϷ�ʽ */

	io_out8(PIC1_ICW1, 0x11  ); /* ��ҪICW4�� ��Ƭ������ ���ش�����ʽ */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15����INT 0x28~0x2f���� */
	io_out8(PIC1_ICW3, 2     ); /* PIC1��IRQ2���� */
	io_out8(PIC1_ICW4, 0x01  ); /* ��ͨȫǶ�� �ǻ��� ���Զ������жϷ�ʽ */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1����ȫ����ֹ */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 ��ֹ��PIC�����ж� */

	return;
}

/* �������Լ��̵��ж�  ��AsmFunc.nas�е�_asm_inthandler21���� */
void inthandler21(int *esp)
{
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* ֪ͨIRQ1�Ѿ�������� */
	data = io_in8(PORT_KEYDAT);	/* ��8042������������ж�������, ��������, ��8042���ٽ������� */
	fifo8_put(&KeyFifo, data);	/* �����յ������ݴ�����̻����������� */
	/*
	struct BOOTINFO *stBootInfo= (struct BOOTINFO *) 0x0ff0;//��ȡ����ʱ�򱣴����Ϣ
	unsigned char *vram=stBootInfo->vram;
	int nXSize=stBootInfo->scrnx;
	int nYSize=stBootInfo->scrny;
	
	char szTemp[40];
	sprintf(szTemp, "KeyTest");
	
	PutFont_Asc(vram, 320, 0, 0, COL_WHITE, szTemp);//�������
	*/
	return;
}

/* ��������PS/2�����ж� */
void inthandler2c(int *esp)
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64);	/* ֪ͨPIC1  IRQ12�Ѿ�������� */
	io_out8(PIC0_OCW2, 0x62);	/* ֪ͨPIC0  IRQ2�Ѿ�������� */
	data = io_in8(PORT_KEYDAT);	/* ��8042������������ж�������, ��������, ��8042���ٽ������� */
	fifo8_put(&MouseFifo, data);/* �����յ������ݴ�����껺���������� */
	return;
}

/* ����IRQ7�ж� ��naskfunc.nas�е�_asm_inthandler27���� */
void inthandler27(int *esp)								
{
	io_out8(PIC0_OCW2, 0x67); /* ֱ�ӷ���EOI���� ��ʾ�жϴ������ */
	return;
}