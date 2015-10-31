#include "common.h"

/* 等待键盘控制电路准备完毕 */
/* 
	事实上它不停地从i8042的64h端口读取Status Register的内容
	然后判断Status Register的bit1是否为0  若为0 说明输入缓冲区是空的
	可以接收CPU发来的命令或数据, 若为1 说明输入缓冲区是满的 无法接收
	CPU发来的命令或数据	
 */
void wait_KBC_sendready(void)
{
	for (;;) 
	{
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) 
		{
			break;
		}
	}
	return;
}


/* 初始化键盘控制电路 */
void Init_Keyboard(void)
{
	wait_KBC_sendready();		/* 清空i8042的输入缓冲区 */
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);	/* 向i8042写入命令 */
												/* 该命令表示准备写入i8042的控制寄存器 
												   下一个通过60h端口写入的字节将被放入
												   i8042的控制寄存器中
												 */
	wait_KBC_sendready();		/* 清空i8042的输入缓冲区 */
	io_out8(PORT_KEYDAT, KBC_MODE);	/* 将KBC_MODE放入i8042的控制寄存器中 
										开启鼠标、键盘
										允许鼠标中断和键盘中断
									*/
	return;
}



/* 激活鼠标 */
void Enable_Mouse(struct MOUSE_DEC *mdec)
{
	wait_KBC_sendready();		/* 清空i8042的输入缓冲区 */
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);	/* 向i8042写入命令 */
												/* 该命令表示将发生0x60端口的参数数据发生给鼠标	*/
	wait_KBC_sendready();		/* 清空i8042的输入缓冲区 */
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);	/* 允许鼠标发数据 */
	mdec->phase = 0;
	return; 	
}

/* 接收鼠标数据 */
int Mouse_Decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) 
	{
		/* 等待鼠标的0xfa阶段 */
		if (dat == 0xfa) 
		{		/* 收到确认信息 */
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) 
	{
		/* 等待鼠标的第一字节阶段 */
		if ((dat & 0xc8) == 0x08) {		/* 判断位移是否溢出(-255~+255) */
			mdec->buf[0] = dat;	
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2)
	{
		/* 等待鼠标的第二字节阶段 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) 
	{
		/* 等待鼠标的第三字节阶段 */
		mdec->buf[2] = dat;
		mdec->phase = 1;				
		mdec->btn = mdec->buf[0] & 0x07;	/* 取BYTE1的低3位 按键状态 */
		mdec->x = mdec->buf[1];				/* 保存x,y的位移 */
		mdec->y = mdec->buf[2];				
		if ((mdec->buf[0] & 0x10) != 0)
		{			/* 判断x的符号位 */
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0)
		{	/* 判断y的符号位 */
			mdec->y |= 0xffffff00;
		}									/* mdec->x和mdec->y中存放的都是补码 */
		/*
			假设x的位移量是-100, -100的补码是1 1001 1100(最高位1在BYTE1中, 低8位是BYTE2)
			初始medc->x中存放的就是1001 1100, 通过((mdec->buf[0] & 0x10) != 0)判断出符号
			位为1，即是负数,则mdec->x = 0xFFFFFF9C,当前的mx是152 = 0x98。那么新的位移量应
			该是152+(-100) = 52.
			在86行有mx += mdec.x; 即0xFFFFFF9C+0x98 = 0x100000034 由于int是4个字节的,所以
			最左边的1会被舍去, 即mx = 0x34 = 52
		 */
		mdec->y = - mdec->y; /* 鼠标的y方向与画面符号相反 */
		return 1;
	}
	return -1; /* 不会到这儿 */
}
