#include "common.h"


struct FIFO8 KeyFifo, MouseFifo;

void HariMain(void)
{
	struct BOOTINFO *stBootInfo= (struct BOOTINFO *) 0x0ff0;//获取启动时候保存的信息
	char s[100];//输出缓冲区
	char szTemp[40],a_MouseCur[256], KeyBuf[32], MouseBuf[128];
	
	unsigned char *vram=stBootInfo->vram;
	int nXSize=stBootInfo->scrnx;
	int nYSize=stBootInfo->scrny;
	
	init_gdtidt();		//初始化GDT, IDT 表
	
	Init_PIC();			/* 初始化PIC */
	io_sti();			/* 打开所有可屏蔽中断 */
	
	fifo8_init(&KeyFifo, 32, KeyBuf);		/* 初始化键盘缓冲区结构体 */
	fifo8_init(&MouseFifo, 128, MouseBuf);	/* 初始化鼠标缓冲区结构体 */
	
	io_out8(PIC0_IMR, 0xf9); /* PIC0(11111001) (打开IRQ1键盘中断和连接从PIC的IRQ2)*/
	io_out8(PIC1_IMR, 0xef); /* PIC1(11101111) (打开PS2鼠标中断 即IRQ12)*/
	
	Init_Keyboard();/*初始化键盘控制电路*/	
	Enable_Mouse();		/* 激活鼠标 */
	
	DrawBack(vram,nXSize,nYSize);//绘制背景
	
	
	sprintf(szTemp, "Screen:(%d, %d)", nXSize, nYSize);
	PutFont_Asc(vram, nXSize, 0, 0, COL_WHITE, szTemp);//输出文字
	
	int mx=(nXSize-16)/2;
	int my=(nYSize-16)/2;
	struct MOUSE_DEC mdec;
	
	Init_MouseCur(a_MouseCur,COL_BACK_BLUE);
	PutBlock(vram,nXSize, 16, 16, mx, my, a_MouseCur, 16);//显示鼠标
	
	int i;
	for (;;) 
	{
		io_cli();		/* 关闭所有可屏蔽中断 */
		if (fifo8_status(&KeyFifo) + fifo8_status(&MouseFifo) == 0) 
		{	/* 如果键盘缓冲区或者鼠标缓冲区中都没有数据 */
			io_stihlt();	/* 开中断并待机 直到下一次中断来临 */
		} 
		else 
		{		/* 键盘或鼠标缓冲区中有数据 */
			if (fifo8_status(&KeyFifo) != 0) 
			{		/* 如果键盘缓冲区中有数据 */
				i = fifo8_get(&KeyFifo);			/* 读取数据 */
				io_sti();							/* 打开所有可屏蔽中断 */
				
				sprintf(s, "%02X", i);				/* 将读取的数据以十六进制形式输出 */
				RectFill(vram, nXSize, COL_WHITE,  0, 16, 15, 31);
				PutFont_Asc(vram, nXSize, 0, 16, COL_BLACK, s);
			} 
			else if (fifo8_status(&MouseFifo) != 0) 
			{	/* 如果鼠标缓冲区中有数据 */
				i = fifo8_get(&MouseFifo);				/* 读取数据 */
				io_sti();								/* 打开所有可屏蔽中断 */
				
				if (Mouse_Decode(&mdec, i) != 0) 
				{	/* 接收鼠标发送的数据 */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);	
					if ((mdec.btn & 0x01) != 0) /* 如果左键被按下 */
					{	
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) /* 如果右键被按下 */
					{	
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) /* 如果滚轮被按下 */
					{	
						s[2] = 'C';
					}
					RectFill(vram, nXSize,COL_WHITE, 32, 16, 32 + 15 * 8 - 1, 31);
					PutFont_Asc(vram, nXSize, 32, 16, COL_BLACK, s);	/* 输出信息 */
					
					RectFill(vram, nXSize,COL_BACK_BLUE, mx, my, mx + 15, my + 15); /* 隐藏鼠标 */
					
					mx += mdec.x;					/* 更新新的鼠标位置 */
					my += mdec.y;
					
					if (mx < 0) /* 鼠标的位置不能小于0,即不能超出屏幕位置 */
					{	
						mx = 0;		
					}
					if (my < 0) 
					{
						my = 0;
					}
					
					if (mx > nXSize - 16) /* 范围控制 ,防止超出屏幕*/
					{	
						mx = nXSize - 16;
					}
					if (my > nYSize - 16) 
					{
						my = nYSize - 16;
					}
					
					sprintf(s, "MousePos:(%3d, %3d)", mx, my);
					RectFill(vram, nXSize, COL_WHITE, 0, 33, 32 + 15 * 8 - 1, 49); /* 隐藏坐标 */
					PutFont_Asc(vram, nXSize, 0, 33, COL_BLACK, s); /* 显示新坐标 */
					
					PutBlock(vram,nXSize, 16, 16, mx, my, a_MouseCur, 16); /* 画出新的鼠标 */
				}
			}
		}	
	}
}

