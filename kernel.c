#include "common.h"

struct FIFO32 SysFifo;
int nKeyData0,nMouseData0;//鼠标键盘再接收数据时会加上的数字（为了合并fifo缓冲区） 


#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

extern struct TIMERCTL timerctl;

/* 任务B */
void task_b_main(struct SHEET *sht_back);
void task_b_main1(struct SHEET *sht_back);
/* Window任务*/
void PutWinTextChar(struct SHEET *sht_back,char *s,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY);
void task_win_main(struct SHEET *sht_back);

/*备注：可能由于测试虚拟机CPU调度策略不同，VM虚拟机 io_stihlt()操作时，
	CPU休眠，count并不自加，而Qemu不会*/
	
void HariMain(void)
{

/*-----相关变量定义----*/	
	struct BOOTINFO *stBootInfo= (struct BOOTINFO *) 0x0ff0;//获取启动时候保存的信息
	char s[100];//输出缓冲区
	char szTemp[40]; //KeyBuf[32], MouseBuf[128];
	int SysBuf[256];
	struct MOUSE_DEC mdec;
	
	/* 	定义一个MEMMAN结构指针(内存管理), 指向MEMMAN_ADDR也就是0x3c0000*/
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;	
	
	struct SHTCTL *shtctl;		/* 图层管理结构指针 */
	struct SHEET *Sht_Back, *Sht_Mouse, *Sht_Win,*Sht_TaskWin[2];	/* 背景以及鼠标的图层指针 */
	unsigned char *Buf_Back, Buf_Mouse[256],*Buf_Win,*Buf_TaskWin[2];
	struct TIMER *timer;

	struct TASK *task_a, *task_b[3];
	//struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;	/* 申请一个段描述符 */
	
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
	nKeyData0=256;
	nMouseData0=512;
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
	shtctl = shtctl_init(memman,vram,nXSize,nYSize);	/* 初始化图层管理结构 */
	Sht_Back  = sheet_alloc(shtctl);		/* 创建背景图层 */
	Sht_Mouse = sheet_alloc(shtctl);		/* 创建鼠标图层 */
	Sht_Win   = sheet_alloc(shtctl);		/* 创建窗口图层 */
	Sht_TaskWin[0]	= sheet_alloc(shtctl);		/* 创建任务1图层 */
	Sht_TaskWin[1]	= sheet_alloc(shtctl);		/* 创建任务2图层 */
	
	Buf_Back  = (unsigned char *) memman_alloc_4k(memman, nXSize * nYSize);	/* 分配内存空间 用于绘制背景*/
	Buf_Win   = (unsigned char *) memman_alloc_4k(memman, 450 * 300);	/* 分配内存空间 用于绘制"窗口 */
	Buf_TaskWin[0] = (unsigned char *) memman_alloc_4k(memman, 170 * 75);
	Buf_TaskWin[1] = (unsigned char *) memman_alloc_4k(memman, 170 * 75);
	
	sheet_setbuf(Sht_Back, Buf_Back, nXSize,nYSize, -1); 			/* 设置背景图层信息 */
	sheet_setbuf(Sht_Mouse, Buf_Mouse, 16, 16, COL_BACK_BLUE);		/* 设置鼠标图层信息 */
	sheet_setbuf(Sht_Win, Buf_Win, 450, 300, -1);					/* 设置窗口图层信息 */
	sheet_setbuf(Sht_TaskWin[0], Buf_TaskWin[0], 170, 75, -1);
	sheet_setbuf(Sht_TaskWin[1], Buf_TaskWin[1], 170, 75, -1);
	
	Init_MouseCur(Buf_Mouse, COL_BACK_BLUE);		/* 初始化鼠标图形 */
	DrawBack(Buf_Back, nXSize, nYSize);	/* 绘制背景 */
	make_window8(Buf_Win, 450, 300, "NoteWindow");	/* 绘制窗口图形 */
	make_window_edit(Buf_Win, 450, 300);	/* 绘制窗口图形 */
	make_window8(Buf_TaskWin[0], 170, 75, "Task1");	/* 绘制窗口图形 */
	make_window8(Buf_TaskWin[1], 170, 75, "Task2");	/* 绘制窗口图形 */
	
	sheet_slide(Sht_Back, 0, 0);	/* 设置背景图层的位置 */
	sheet_slide(Sht_Win,  85, 160);				/* 设置窗口图层的位置 */
	sheet_slide(Sht_TaskWin[0], nYSize - 20, 60);				/* 设置窗口图层的位置 */
	sheet_slide(Sht_TaskWin[1], nYSize - 20, 140);				/* 设置窗口图层的位置 */
	
	int mx = (nXSize- 16) / 2; /* 计算鼠标图形在屏幕上的位置 它在整个桌面的中心位置 */
	int my = (nYSize - 28 - 16) / 2;	
	sheet_slide(Sht_Mouse, mx, my);	/* 设置鼠标图层的位置 */
	
	sheet_updown(Sht_Back,  0);		/* 调整背景图层和鼠标图层的高度 */
	sheet_updown(Sht_TaskWin[0],   1);	
	sheet_updown(Sht_TaskWin[1],   2);	
	sheet_updown(Sht_Win,   -1);		/* 调整默认窗口 */
	sheet_updown(Sht_Mouse, 4);		/* 并且会显示背景与鼠标图层 */
/*---任务切换相关---*/
	int i;
	/* 任务切换初始化(*/
	task_a = task_init(memman);//task_a 是内核任务
	fifo32_init(&SysFifo, 256, SysBuf,task_a);

	task_run(task_a, -1, 2);		/* 设置任务a的level和优先级 */
	
	for(i=0;i<3;i++)
	{
		task_b[i]=task_alloc();//分配一个任务
	
		task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;	/* 创建任务B的堆栈，64K */;
		
		task_b[i]->tss.es = 1 * 8;
		task_b[i]->tss.cs = 2 * 8;
		task_b[i]->tss.ss = 1 * 8;
		task_b[i]->tss.ds = 1 * 8;
		task_b[i]->tss.fs = 1 * 8;
		task_b[i]->tss.gs = 1 * 8;
	}
	
	task_b[0]->tss.eip = (int) &task_b_main;		/* 设置任务B的寄存器 */
	task_b[1]->tss.eip = (int) &task_b_main1;		/* 设置任务B的寄存器 */
	task_b[2]->tss.eip = (int) &task_win_main;		/* 设置任务Window的寄存器 */

	/* 先将task_b_esp + 4转换成int类型的指针  即(int *) (task_b_esp + 4) */
	/* 再将(int) sht_back赋值到该地址处*((int *) (task_b_esp + 4)) */
	*((int *) (task_b[0]->tss.esp + 4)) = (int) Sht_TaskWin[0];
	*((int *) (task_b[1]->tss.esp + 4)) = (int) Sht_TaskWin[1];
	*((int *) (task_b[2]->tss.esp + 4)) = (int) Sht_Win;
	
	//task_run(task_a, 10);//分配给内核最高优先级
	
	//task_run(task_b[0], 1);			
	//task_run(task_b[1], 2);	
	//task_run(task_b[2], 2);
	
	
/*----显示系统信息----*/
	//sprintf(s, "(%3d, %3d)", mx, my);
	sprintf(szTemp, "Screen:(%d, %d)", nXSize, nYSize);
	putfonts8_asc_sht(Sht_Back, 0, 0,COL_RED,COL_WHITE, szTemp, 17);//在图层上显示文字
	
	sprintf(szTemp, "MemMaxSize:%d MB", nMemMaxSize/(1024*1024));	 
	putfonts8_asc_sht(Sht_Back, 0, 51,COL_BLACK,COL_GREEN, szTemp, 16);//在图层上显示文字
	
	sprintf(szTemp, "MemFree:%d KB", memman_total(memman) /1024);	
	putfonts8_asc_sht(Sht_Back, 0, 68,COL_BLACK,COL_GREEN, szTemp, 16);//在图层上显示文字


/*----定时器设置----*/	
	timer = timer_alloc();					/* 创建定时器 */
	timer_init(timer, &SysFifo, 100);		/* 定时器初始化 */
	timer_settime(timer, 100);				/* 定时器设置 */	
	
	
/*---内核循环---*/	
	for (;;) 
	{
		io_cli();		/* 关闭所有可屏蔽中断 */
		
		/* 系统缓冲区没有数据 */
		if(fifo32_status(&SysFifo)== 0) 
		{	
			//io_stihlt();	/* 开中断并待机 直到下一次中断来临 */
			task_sleep(task_a);//休眠内核任务，在fifo有数据的时候才被重新激活
			io_sti();
		} 
		else 
		{	
			i = fifo32_get(&SysFifo);
			io_sti();//打开中断
		
			if(nKeyData0<=i && i<=nKeyData0+0xFF)/*键盘缓冲区有数据 */
			{		
				i-=nKeyData0;		
				sprintf(s, "%02X", i);	
				putfonts8_asc_sht(Sht_Back, 0, 16,COL_BLACK,COL_GREEN, s, 2);//在图层上显示文字
			} 
			else if (nMouseData0<=i && i<=nMouseData0+0xFF) /* 鼠标缓冲区有数据 */				
			{	

				i-=nMouseData0;
				if (Mouse_Decode(&mdec, i) != 0) 
				{	/* 接收鼠标发送的数据 */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);	
					if ((mdec.btn & 0x01) != 0) /* 如果左键被按下 */
					{	
						s[1] = 'L';
						sheet_slide(Sht_Win, mx, my);
					}
					if ((mdec.btn & 0x02) != 0) /* 如果右键被按下 */
					{	
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) /* 如果滚轮被按下 */
					{	
						s[2] = 'C';
					}

					putfonts8_asc_sht(Sht_Back, 32, 16,COL_BLACK,COL_GREEN, s, 15);//在图层上显示文字
					
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
					putfonts8_asc_sht(Sht_Back, 0, 33,COL_BLACK,COL_GREEN, s, 19);//在图层上显示文字
					
					sheet_slide(Sht_Mouse, mx, my);	/* 更新鼠标图层的位置并显示新的鼠标图层 */
				
				}
			}
			else if (i == 100) /* 定时器数据 ,100是设置的写入data*/
			{	
				
				//-----------任务栏秒数显示----------------------------------
				sprintf(s, "%03d Sec", timerctl.count/100);
				putfonts8_asc_sht(Sht_Back, nXSize-60, nYSize-23,COL_WHITE,COL_BLACK, s, 9);//在图层上显示文字
				
				if(timerctl.count/100==2)//3秒时候开始
				{
					task_run(task_b[0], 2,1);			
					task_run(task_b[1], 2,2);
					
					//sheet_updown(Sht_Win,   3);		/* 调整默认窗口 */  
					//task_run(task_b[2],2,2);
				}
				else if(timerctl.count/100==7)//3秒时候停止
				{
					task_sleep(task_b[0]);
					task_sleep(task_b[1]);
					
					sheet_updown(Sht_Win,   3);		/* 调整默认窗口 */  
					task_run(task_b[2],2,2);
				}

				timer_settime(timer, 100);				/* 定时器设置 */	

			}
		}	
	}
}

/*-----应用任务---------------------------------------*/

/*输出一个字符到窗体文本框*/
void PutWinTextChar(struct SHEET *sht_back,char *s,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY)
{
	char szChar[2];//存取单个字符，临时变量
	
	szChar[0]=*s;
	szChar[1]=0;
					
	if(szChar[0]=='\n')//暂且用这个代表换行。。以后再换
	{
		RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, *nPos_CurX, *nPos_CurY, *nPos_CurX+1 , *nPos_CurY + 16);
		sheet_refresh(sht_back, *nPos_CurX, *nPos_CurY, *nPos_CurX+10+1 , *nPos_CurY +16+1);

		if(*nPos_CurY<nYSize-40)
		{
			*nPos_CurX=12;
			*nPos_CurY+=18;
		}
		return;
	}
	
	if(*nPos_CurX<nXSize-20)//在一行内可以输出
	{				
	
		/*光标绘制 白色*/	
		putfonts8_asc_sht(sht_back, *nPos_CurX, *nPos_CurY,COL_BLACK,COL_WHITE, szChar, 1);//在图层上显示文字
			
		//nTextSel++;		//取字符 位置加1					
		*nPos_CurX+=8;	//光标移至下一个字符位置
						
		if(*nPos_CurX>nXSize-20)//超过一行
		{
			if(*nPos_CurY<nYSize-40)//换行后不会超过底部
			{
				*nPos_CurX=12;
				*nPos_CurY+=18;
				putfonts8_asc_sht(sht_back, *nPos_CurX, *nPos_CurY,COL_BLACK,COL_WHITE, szChar, 1);//在图层上显示文字
			}
		}
										
	}

	
}

/* Window任务*/
void task_win_main(struct SHEET *sht_back)
{
	struct FIFO32 MsgFifo;//任务消息队列
	struct TIMER *timer_text_Cur;//光标闪烁定时器
	struct TIMER *timer_put;//介绍文字输出定时器
	struct TASK *task = task_now();//当前任务，即本身
	
	int nXSize=450;//窗体宽度
	int nYSize=300;//窗体高度

	int nCurTime=30;//光标闪烁间隔
	int nPutTime=3;//介绍文字输出间隔
	
	int MsgFifo_buf[128];
	int i;
		
	int nPos_CurX=12;
	int nPos_CurY=30;
	
	char szChar[2];//存取单个字符，临时变量
	int nTextSel=0,nTextLine=0;//正在输出的文字位置,行数
	
	char *szText[5]={"          Welcome to use SmlOS.","-       If you hava any question.","-       You can contact me.","-       DaterLove","-       QQ:306463830"};
	
	fifo32_init(&MsgFifo, 128, MsgFifo_buf,task);
	timer_text_Cur = timer_alloc();	
	timer_put = timer_alloc();	
	timer_init(timer_text_Cur, &MsgFifo, 1);
	timer_init(timer_put, &MsgFifo, 2);
	timer_settime(timer_text_Cur, nCurTime);
	timer_settime(timer_put, 50);
	
	for (;;) 
	{
		io_cli();
		if (fifo32_status(&MsgFifo)== 0) 
		{
			task_sleep(task);					/* 睡眠自己 */
			io_sti();
		} 
		else 
		{
			
			i = fifo32_get(&MsgFifo);
			io_sti();//打开中断
			
			if (i == 1)//光标定时器
			{
					/*光标绘制 黑色*/
				RectFill(sht_back->buf, sht_back->bxsize, COL_PURPLE, nPos_CurX, nPos_CurY, nPos_CurX+1 , nPos_CurY + 16);
				sheet_refresh(sht_back, nPos_CurX, nPos_CurY, nPos_CurX+10+1 , nPos_CurY + 16+1);
				timer_init(timer_text_Cur, &MsgFifo, 0);
				timer_settime(timer_text_Cur, nCurTime);	

			}
			else if(i==0)//光标定时器
			{
				/*光标绘制 白色*/
				RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, nPos_CurX, nPos_CurY, nPos_CurX+1 , nPos_CurY + 16);
				sheet_refresh(sht_back, nPos_CurX, nPos_CurY, nPos_CurX+10+1 , nPos_CurY +16+1);
				timer_init(timer_text_Cur, &MsgFifo, 1);
				timer_settime(timer_text_Cur, nCurTime);
			}
			else if((i==2))//文字输出定时器
			{
				if(nTextLine<5)//字符行数
				{
					szChar[0]=*(szText[nTextLine]+nTextSel);
					szChar[1]=0;
					
					if(szChar[0]!='\0')
					{
						PutWinTextChar(sht_back,szChar,nXSize,nYSize,&nPos_CurX,&nPos_CurY);//输出文字
						nTextSel++;
					}
					else
					{
						//换行
						szChar[0]='\n';
						szChar[1]=0;
						PutWinTextChar(sht_back,szChar,nXSize,nYSize,&nPos_CurX,&nPos_CurY);
						nTextLine++;
						nTextSel=0;
					}

					timer_settime(timer_put, nPutTime);
				}
			}
			
		}
	}
	
}
/* 任务B(Task1)*/
void task_b_main(struct SHEET *sht_back)
{
	struct FIFO32 fifo;
	
	struct TIMER *timer_put;
	int i, fifobuf[128], count = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf,0);
	
	timer_put = timer_alloc();		
	timer_init(timer_put, &fifo, 1);
	timer_settime(timer_put, 50);
	
	putfonts8_asc_sht(sht_back, 22, 28,COL_BLACK,COL_APPLE_GRREN, " Priority:1ms", 16);//在图层上显示文字
	
	for (;;) 
	{
		count++;	
		io_cli();
		
		if (fifo32_status(&fifo) == 0) 
		{
			io_sti();
			//io_stihlt();
		} 
		else 
		{
			i = fifo32_get(&fifo);
			io_sti();
			if (i == 1) 
			{
				sprintf(s, " Count:%d", count);
				putfonts8_asc_sht(sht_back, 22, 50,COL_BLACK,COL_APPLE_GRREN, s, 16);//在图层上显示文字
				
				timer_settime(timer_put, 10);		
			}
		}
	}
	
}
/* 任务B(Task2)*/
void task_b_main1(struct SHEET *sht_back)
{
	struct FIFO32 fifo;
	
	struct TIMER *timer_put;
	int i, fifobuf[128], count = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf,0);
	
	timer_put = timer_alloc();		
	timer_init(timer_put, &fifo, 1);
	timer_settime(timer_put, 50);
	
	putfonts8_asc_sht(sht_back, 22, 28,COL_BLACK,COL_APPLE_GRREN, " Priority:2ms", 16);//在图层上显示文字
	
	for (;;) 
	{
		count++;	
		io_cli();
		
		if (fifo32_status(&fifo) == 0) 
		{
			io_sti();
			//io_stihlt();
		} 
		else 
		{
			i = fifo32_get(&fifo);
			io_sti();
			if (i == 1) 
			{
				sprintf(s, " Count:%d", count);
				putfonts8_asc_sht(sht_back, 22, 50,COL_BLACK,COL_APPLE_GRREN, s, 16);//在图层上显示文字
				
				timer_settime(timer_put, 10);		
			}
		}
	}
	
}



