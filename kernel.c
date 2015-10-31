#include "common.h"

#define TEST_ING 0

struct FIFO32 SysFifo;
int nKeyData0,nMouseData0;//鼠标键盘再接收数据时会加上的数字（为了合并fifo缓冲区） 


#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

extern struct TIMERCTL timerctl;



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
	
	//窗体移动相关变量
	//
	int mmx=-1,mmy=-1;
	struct SHEET *sht=0;
	
	/* 没有按shift */
	static char keytable0[0x80] = 
	{
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	/* 按下shift后 */
	static char keytable1[0x80] = 
	{
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	int key_shift = 0;
	int key_leds = (stBootInfo->leds >> 4) & 7;
	int key_wait = -1;
	
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

	//io_out8(PIC1_IMR, 0xef); /* PIC1(11101111) (打开PS2鼠标中断 即IRQ12)*/	
	Init_Keyboard();/*初始化键盘控制电路*/	
	Enable_Mouse(&mdec);		/* 激活鼠标 */

/*----内存操作----*/
	/* 检测4M~3G-1的内存,4M之前被占用，不能检测 */
	int nMemMaxSize=memtest(0x00400000, 0xbfffffff);	
	memman_init(memman);	/* 初始化内存管理结构 */
	
	/* 这段内存是4K~636K-1,这段内存是4K~636K-1,映像刚开始载入内存的内容，包括引导程序那部分*/
	memman_free(memman, 0x00001000, 0x0009e000); 	/* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, nMemMaxSize - 0x00400000);	/* 4M~内存实际大小,4M之前用于保存相关数据，预留了很多空间备用 */


	
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
	sprintf(szTemp, " Screen:(%d, %d)", nXSize, nYSize);
	putfonts8_asc_sht(Sht_Back, 0, 0,COL_RED,COL_WHITE, szTemp, 19);//在图层上显示文字
	
	sprintf(szTemp, "MemMaxSize:%d MB", nMemMaxSize/(1024*1024));	 
	putfonts8_asc_sht(Sht_Back, 0, 57,COL_BLACK,COL_GREEN, szTemp, 16);//在图层上显示文字
	
	sprintf(szTemp, "MemFree:%d KB", memman_total(memman) /1024);	
	putfonts8_asc_sht(Sht_Back, 0, 76,COL_BLACK,COL_GREEN, szTemp, 16);//在图层上显示文字


/*----定时器设置----*/	
	timer = timer_alloc();					/* 创建定时器 */
	timer_init(timer, &SysFifo, 100);		/* 定时器初始化 */
	timer_settime(timer, 100);				/* 定时器设置 */	
	
	unsigned char cTemp=0x25;//颜色测试语句
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
				putfonts8_asc_sht(Sht_Back, 0, 20,COL_BLACK,COL_GREEN, s, 2);//在图层上显示文字
			//----------------------------------------------------------------------------------------
				if (i < 0x80) 
				{ 
					if (key_shift == 0)/* 如果shift没有被按下 */ 
					{	
						s[0] = keytable0[i];		/* 将键值转换成字符的ASCII码 */
					} 
					else/* 如果shift被按下了 */ 
					{				
						s[0] = keytable1[i];		/* 将键值转换成字符的ASCII码 */
					}
				} 
				else 
				{	
					s[0] = 0;
				}
				
				if ('A' <= s[0] && s[0] <= 'Z')/* 如果输入的是英文字母 */ 
				{	
					/* CapsLock为OFF并且Shift为OFF 或者 CapsLock为ON并且Shift为ON */
					if (((key_leds & 4) == 0 && key_shift == 0) ||((key_leds & 4) != 0 && key_shift != 0)) 
					{
						s[0] += 0x20;	/* 将大写字母转换成小写字母 */
										/* 同一个字母的大小写ASCII码相差0x20 */
					}
				}
				
				if (i == 0x0e)/* 退格键 */ 
				{	
					if(task_b[2]->fifo)//如果fifo存在
					{
						fifo32_put(task_b[2]->fifo, 8 + 256);
					}
				}
				else if (i == 0x1c)/* 换行键 */ 
				{	
					if(task_b[2]->fifo)//如果fifo存在
					{
						fifo32_put(task_b[2]->fifo, 7 + 256);
					}
				}
				else if (s[0] != 0)  /* 一般字符 */
				{
					if(task_b[2]->fifo)//如果fifo存在
					{
						fifo32_put(task_b[2]->fifo, s[0] + 256);
					}
				}	
			
				/* key_shift变量的bit0和bit1分别标识了左shift和右shift的开启与关闭状态 */
				if (i == 0x2a)/* 左shift ON */ 
				{	
					key_shift |= 1;
				}
				if (i == 0x36)/* 右shift ON */
				{	
					key_shift |= 2;
				}
				if (i == 0xaa)/* 左shift OFF */ 
				{	
					key_shift &= ~1;
				}
				if (i == 0xb6)/* 右shift OFF */ 
				{	
					key_shift &= ~2;
				}
				
				// 0xfa是ACK信息
				if (i == 0xfa) // 键盘成功接收到数据 
				{	
					key_wait = -1;	//等于-1表示可以发送指令 
					putfonts8_asc_sht(Sht_Back, 20, 18,COL_BLACK,COL_RED, s, 2);//在图层上显示文字
				}
				if (i == 0xfe)// 键盘没有成功接收到数据 
				{	
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, key_wait);	// 重新发送上次的指令 
					putfonts8_asc_sht(Sht_Back, 20, 16,COL_BLACK,COL_YELLOW, s, 2);//在图层上显示文字
				}
				
			//----------------------------------------------------------------------------------------
				
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
						//----移动窗体到最顶层-------------------------------------------
						int j,x,y;
						if(mmx<0)//普通模式（mmx负数）
						{
							//遍历图层
							for (j = shtctl->top - 1; j > 0; j--) 
							{
								sht = shtctl->p_sheets[j];
								//vx0, vy0图层左上角的坐标
								x = mx - sht->vx0;//鼠标与窗口左上角长度距离
								y = my - sht->vy0;//鼠标与窗口左上角高度距离
								//在图层的大小范围内
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) 
								{
									//按中 关闭按钮，隐藏窗体
									if(x > sht->bxsize-25 && x < sht->bxsize-5 && y > 4 && y < 17)
									{
										sheet_updown(sht, -1);
										break;
									}
									//不是透明色
									if (sht->buf[y * sht->bxsize + x] != sht->col_inv)
									{
										sheet_updown(sht, shtctl->top - 1);
					
										if(y<22)//在标题栏中，则进入拖动模式
										{
											mmx=mx;
											mmy=my;
										}
										break;
									}
										
								}
							}
						}
						else//进入拖动模式
						{
							x=mx-mmx;//计算鼠标移动距离
							y=my-mmy;//这里mmx，mmx是上一次鼠标位置
							//移动船体
							sheet_slide(sht, sht->vx0+x, sht->vy0+y);
							mmx=mx;
							mmy=my;
						}
						
					}
					else
					{
						mmx=-1;//返回通常模式（左键放开时）
					}
					//-----------------------------------------------------------------
					
					if ((mdec.btn & 0x02) != 0) /* 如果右键被按下 */
					{	
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) /* 如果滚轮被按下 */
					{	
						s[2] = 'C';
					}

					putfonts8_asc_sht(Sht_Back, 32, 20,COL_BLACK,COL_GREEN, s, 15);//在图层上显示文字
					
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
					
					sheet_slide(Sht_Mouse, mx, my);	/* 更新鼠标图层的位置并显示新的鼠标图层 */
					sprintf(s, "MousePos:(%3d, %3d)", mx, my);
					putfonts8_asc_sht(Sht_Back, 0, 39,COL_BLACK,COL_GREEN, s, 19);//在图层上显示文字
					
					
				
				}
			}
			else if (i == 100) /* 定时器数据 ,100是设置的写入data*/
			{	//-----------任务栏秒数显示----------------------------------
				sprintf(s, "%03d Sec", timerctl.count/100);
				putfonts8_asc_sht(Sht_Back, nXSize-60, nYSize-23,COL_WHITE,COL_BLACK, s, 9);//在图层上显示文字
				
				if(timerctl.count/100==2)//2秒时候开始
				{
					//根据预处理命令判断是否在测试中
					#if TEST_ING
						sheet_updown(Sht_Win,shtctl->top-1);	//弹出笔记本窗口 
						task_run(task_b[2],2,2);
					#else
						task_run(task_b[0], 2,1);			
						task_run(task_b[1], 2,2);
					#endif 
					
				}
				else if(timerctl.count/100==7)//7秒时候停止
				{
					//根据预处理命令判断是否在测试中
					#if TEST_ING
					#else
						task_sleep(task_b[0]);
						task_sleep(task_b[1]);
					#endif 
				}
				else if(timerctl.count/100==8)
				{
					#if TEST_ING
					#else
				
						sheet_updown(Sht_Win,shtctl->top-1);		//弹出笔记本窗口  
						task_run(task_b[2],2,2);
					#endif 
				}
				/*颜色测试语句
				sprintf(s, "%X", cTemp);
				
				RectFill(vram,nXSize,cTemp,0,0,nXSize,nYSize);
				PutFont_Asc(vram, nXSize, 100, 250, COL_WHITE, s);
				cTemp++;*/
				
				timer_settime(timer, 100);				/* 定时器设置 */	

			}
		}	
	}
}





