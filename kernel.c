#include "common.h"


struct FIFO8 KeyFifo, MouseFifo;

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

extern struct TIMERCTL timerctl;

void HariMain(void)
{
/*-----相关变量定义----*/	
	struct BOOTINFO *stBootInfo= (struct BOOTINFO *) 0x0ff0;//获取启动时候保存的信息
	char s[100];//输出缓冲区
	char szTemp[40],a_MouseCur[256], KeyBuf[32], MouseBuf[128];
	struct MOUSE_DEC mdec;
	
	/* 	定义一个MEMMAN结构指针, 指向MEMMAN_ADDR也就是0x3c0000*/
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;	
	
	struct SHTCTL *shtctl;		/* 图层管理结构指针 */
	struct SHEET *Sht_Back, *Sht_Mouse, *Sht_Win;	/* 背景以及鼠标的图层指针 */
	unsigned char *Buf_Back, Buf_Mouse[256],*Buf_Win;;

	char timerbuf[8], timerbuf2[8], timerbuf3[8];
	struct FIFO8 timerfifo, timerfifo2, timerfifo3;	/* 用于定时器的队列 */
	struct TIMER *timer, *timer2, *timer3;
	
/*-----系统初始化操作----*/
	unsigned char *vram=stBootInfo->vram;
	int nXSize=stBootInfo->scrnx;
	int nYSize=stBootInfo->scrny;
	init_palette();		/* 初始化调色板 */
	init_gdtidt();		//初始化GDT, IDT 表
	
	
	Init_PIC();			/* 初始化PIC */
	io_sti();			/* 打开所有可屏蔽中断 */

	init_pit();			/*初始化 时钟中断*/
	io_out8(PIC0_IMR, 0xf8); /* PIC0(11111000) (打开IRQ0时钟中断、IRQ1键盘中断和连接从PIC的IRQ2)*/
	io_out8(PIC1_IMR, 0xef); /* PIC1(11101111) (打开PS2鼠标中断 即IRQ12)*/
	
/*----初始化鼠标键盘操作----*/
	fifo8_init(&KeyFifo, 32, KeyBuf);		/* 初始化键盘缓冲区结构体 */
	fifo8_init(&MouseFifo, 128, MouseBuf);	/* 初始化鼠标缓冲区结构体 */
//	io_out8(PIC0_IMR, 0xf9); /* 这个bug调我N久，注视下留个纪念。。*/

	io_out8(PIC1_IMR, 0xef); /* PIC1(11101111) (打开PS2鼠标中断 即IRQ12)*/	
	Init_Keyboard();/*初始化键盘控制电路*/	
	Enable_Mouse();		/* 激活鼠标 */

/*----内存操作----*/
	/* 检测4M~3G-1的内存,4M之前被占用，不能检测 */
	int nMemMaxSize=memtest(0x00400000, 0xbfffffff);	
	memman_init(memman);	/* 初始化内存管理结构 */
	
	/* 这段内存是4K~636K-1,这段内存是4K~636K-1,映像刚开始载入内存的内容，包括引导程序那部分*/
	memman_free(memman, 0x00001000, 0x0009e000); 	/* 0x00001000 - 0x0009efff */
	
	memman_free(memman, 0x00400000, nMemMaxSize - 0x00400000);	/* 4M~内存实际大小 */
	
/*----图形操作----*/
	/*DrawBack(vram,nXSize,nYSize);//绘制背景
	sprintf(szTemp, "Screen:(%d, %d)", nXSize, nYSize);
	PutFont_Asc(vram, nXSize, 0, 0, COL_WHITE, szTemp);//输出屏幕大小文字
	
	int mx=(nXSize-16)/2;
	int my=(nYSize-16)/2;
	
	Init_MouseCur(a_MouseCur,COL_BACK_BLUE);//初始化鼠标图形
	PutBlock(vram,nXSize, 16, 16, mx, my, a_MouseCur, 16);//显示鼠标
	*/
	shtctl = shtctl_init(memman,vram,nXSize,nYSize);	/* 初始化图层管理结构 */
	Sht_Back  = sheet_alloc(shtctl);		/* 创建背景图层 */
	Sht_Mouse = sheet_alloc(shtctl);		/* 创建鼠标图层 */
	Sht_Win   = sheet_alloc(shtctl);		/* 创建窗口图层 */
	
	Buf_Back  = (unsigned char *) memman_alloc_4k(memman, nXSize * nYSize);	/* 分配内存空间 用于绘制背景*/
	Buf_Win   = (unsigned char *) memman_alloc_4k(memman, 160 * 52);	/* 分配内存空间 用于绘制"窗口 */
	
	sheet_setbuf(Sht_Back, Buf_Back, nXSize,nYSize, -1); /* 设置背景图层信息 */
	sheet_setbuf(Sht_Mouse, Buf_Mouse, 16, 16, COL_BACK_BLUE);		/* 设置鼠标图层信息 */
	sheet_setbuf(Sht_Win, Buf_Win, 160, 52, -1);						/* 设置窗口图层信息 */
	
	Init_MouseCur(Buf_Mouse, COL_BACK_BLUE);		/* 初始化鼠标图形 */
	DrawBack(Buf_Back, nXSize, nYSize);	/* 绘制背景 */
	make_window8(Buf_Win, 160, 52, "Counter");	/* 绘制窗口图形 */
	
	sheet_slide(Sht_Back, 0, 0);	/* 设置背景图层的位置 */
	sheet_slide(Sht_Win, 180, 72);				/* 设置窗口图层的位置 */
	
	int mx = (nXSize- 16) / 2; /* 计算鼠标图形在屏幕上的位置 它在整个桌面的中心位置 */
	int my = (nYSize - 28 - 16) / 2;	
	sheet_slide(Sht_Mouse, mx, my);	/* 设置鼠标图层的位置 */
	
	sheet_updown(Sht_Back,  0);		/* 调整背景图层和鼠标图层的高度 */
	sheet_updown(Sht_Win,   1);		/* 调整默认窗口 */
	sheet_updown(Sht_Mouse, 2);		/* 并且会显示背景与鼠标图层 */
	
	
/*----显示信息----*/
	sprintf(s, "(%3d, %3d)", mx, my);
	sprintf(szTemp, "Screen:(%d, %d)", nXSize, nYSize);
	PutFont_Asc(Buf_Back, nXSize, 0, 0, COL_WHITE, szTemp);//输出屏幕大小文字
	sheet_refresh(Sht_Back, 0, 0, 140, 16);
	
	sprintf(szTemp, "MemMaxSize:%d MB", nMemMaxSize/(1024*1024));
	//sprintf(szTemp, "MemMaxSize:%X MB", vram);
	RectFill(Buf_Back, nXSize, COL_WHITE, 0, 51, 130, 66); 
	PutFont_Asc(Buf_Back, nXSize, 0, 51, COL_BLACK, szTemp); // 显示内存信息 
	sheet_refresh(Sht_Back, 0, 51, 130+1, 66+1);	 
	
	sprintf(szTemp, "MemFree:%d KB", memman_total(memman) /1024);
	RectFill(Buf_Back, nXSize, COL_WHITE, 0, 68, 130, 83); 
	PutFont_Asc(Buf_Back, nXSize, 0, 68, COL_BLACK, szTemp); // 显示内存信息 
	sheet_refresh(Sht_Back, 0, 68, 130+1, 83+1);	

/*----定时器设置----*/
	fifo8_init(&timerfifo, 8, timerbuf);	/* 初始化队列结构 */
	timer = timer_alloc();					/* 创建定时器 */
	timer_init(timer, &timerfifo, 1);		/* 定时器初始化 */
	timer_settime(timer, 300);				/* 定时器设置 */	
					
	int i;
	int count=0;
	/*备注：可能由于测试虚拟机CPU调度策略不同，VM虚拟机 io_stihlt()操作时，
	CPU休眠，count并不自加，而Qemu不会*/
	for (;;) 
	{
			/*
		//调试语句段
		count++;		//计数器加1 
		sprintf(s, "%010d", timerctl.count);
		RectFill(Buf_Win, 160, COL_APPLE_GRREN, 40, 28, 119, 43);
		PutFont_Asc(Buf_Win, 160, 40, 28, COL_BLACK, s);
		sheet_refresh(Sht_Win, 40, 28, 120, 44);		//在窗口中显示计数器的值 
		*/
		
		io_cli();		/* 关闭所有可屏蔽中断 */
		/* 如果键盘缓冲区或者鼠标缓冲区中都没有数据 */
		if (fifo8_status(&KeyFifo) + fifo8_status(&MouseFifo) +fifo8_status(&timerfifo)== 0) 
		{	
			io_stihlt();	/* 开中断并待机 直到下一次中断来临 */
		} 
		else 
		{		/* 键盘或鼠标缓冲区中有数据 */
			if (fifo8_status(&KeyFifo) != 0) 
			{		/* 如果键盘缓冲区中有数据 */
				i = fifo8_get(&KeyFifo);			/* 读取数据 */
				io_sti();							/* 打开所有可屏蔽中断 */
				
				sprintf(s, "%02X", i);				/* 将读取的数据以十六进制形式输出 */
				RectFill(Buf_Back, nXSize, COL_WHITE,  0, 16, 15, 31);
				PutFont_Asc(Buf_Back, nXSize, 0, 16, COL_BLACK, s);
				
				sheet_refresh(Sht_Back,  0, 16, 15+1, 31+1);		/* 刷新sht_back图层缓冲区中的某一矩形区域 */
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
					RectFill(Buf_Back, nXSize,COL_WHITE, 32, 16, 32 + 15 * 8 - 1, 31);
					PutFont_Asc(Buf_Back, nXSize, 32, 16, COL_BLACK, s);	/* 输出信息 */
					sheet_refresh(Sht_Back, 32, 16, 32 + 15 * 8+1 , 31+1);
					
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
					
					if (mx > nXSize - 1) /* 范围控制 ,防止超出屏幕*/
					{	
						mx = nXSize - 1;
					}
					if (my > nYSize-1 ) 
					{
						my = nYSize-1;
					}
					
					sprintf(s, "MousePos:(%3d, %3d)", mx, my);
					RectFill(Buf_Back, nXSize, COL_WHITE, 0, 33, 32 + 15 * 8 - 1, 49); /* 隐藏坐标 */
					PutFont_Asc(Buf_Back, nXSize, 0, 33, COL_BLACK, s); /* 显示新坐标 */
					
					/*刷新sht_back图层缓冲区中的某一矩形区域以显示上面的更改 */
					sheet_refresh(Sht_Back, 0, 33, 32 + 15 * 8+1, 50);			
					
					sheet_slide(Sht_Mouse, mx, my);	/* 更新鼠标图层的位置并显示新的鼠标图层 */
				
				}
			}
			else if (fifo8_status(&timerfifo) != 0) /* 定时器1有数据 */
			{	
				i = fifo8_get(&timerfifo); /* 读入数据 */
				io_sti();				/* 打开所有可屏蔽中断 */
				sprintf(s, "Timer");
				RectFill(Buf_Win, 160, COL_APPLE_GRREN, 40, 28, 119, 43);
				PutFont_Asc(Buf_Win, 160, 40, 28, COL_BLACK, s);
				sheet_refresh(Sht_Win, 40, 28, 120, 44);		//在窗口中显示计数器的值 
			}
		}	
	}
}

