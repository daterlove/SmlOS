#include "common.h"


/* 初始化PIC */
void Init_PIC(void)
{
	io_out8(PIC0_IMR,  0xff  ); /* 禁止主PIC所有中断 */
	io_out8(PIC1_IMR,  0xff  ); /* 禁止从PIC所有中断 */

	io_out8(PIC0_ICW1, 0x11  ); /* 需要ICW4， 多片级联， 边沿触发方式 */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7由于INT 0x20~0x27接收 */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1由IRQ2连接 */
	io_out8(PIC0_ICW4, 0x01  ); /* 普通全嵌套 非缓冲 非自动结束中断方式 */

	io_out8(PIC1_ICW1, 0x11  ); /* 需要ICW4， 多片级联， 边沿触发方式 */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15由于INT 0x28~0x2f接收 */
	io_out8(PIC1_ICW3, 2     ); /* PIC1由IRQ2连接 */
	io_out8(PIC1_ICW4, 0x01  ); /* 普通全嵌套 非缓冲 非自动结束中断方式 */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1以外全部禁止 */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 禁止从PIC所有中断 */

	return;
}

/* 处理来自键盘的中断  由AsmFunc.nas中的_asm_inthandler21调用 */
void inthandler21(int *esp)
{
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* 通知IRQ1已经受理完毕 */
	data = io_in8(PORT_KEYDAT);	/* 从8042的输出缓冲区中读出数据, 若不读出, 则8042不再接收数据 */
	fifo8_put(&KeyFifo, data);	/* 将接收到的数据存入键盘缓冲区队列中 */
	/*
	struct BOOTINFO *stBootInfo= (struct BOOTINFO *) 0x0ff0;//获取启动时候保存的信息
	unsigned char *vram=stBootInfo->vram;
	int nXSize=stBootInfo->scrnx;
	int nYSize=stBootInfo->scrny;
	
	char szTemp[40];
	sprintf(szTemp, "KeyTest");
	
	PutFont_Asc(vram, 320, 0, 0, COL_WHITE, szTemp);//输出文字
	*/
	return;
}

/* 处理来自PS/2鼠标的中断 */
void inthandler2c(int *esp)
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64);	/* 通知PIC1  IRQ12已经受理完毕 */
	io_out8(PIC0_OCW2, 0x62);	/* 通知PIC0  IRQ2已经受理完毕 */
	data = io_in8(PORT_KEYDAT);	/* 从8042的输出缓冲区中读出数据, 若不读出, 则8042不再接收数据 */
	fifo8_put(&MouseFifo, data);/* 将接收到的数据存入鼠标缓冲区队列中 */
	return;
}

/* 处理IRQ7中断 由naskfunc.nas中的_asm_inthandler27调用 */
void inthandler27(int *esp)								
{
	io_out8(PIC0_OCW2, 0x67); /* 直接发送EOI命令 表示中断处理结束 */
	return;
}