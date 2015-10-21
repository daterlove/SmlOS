#include "common.h"

/*-----应用任务---------------------------------------*/

/*输出一个字符到窗体文本框*/
void PutWinTextChar(struct SHEET *sht_back,char *s,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY)
{
	char szChar[2];//存取单个字符，临时变量
	szChar[0]=*s;
	szChar[1]=0;
					
	if(szChar[0]=='\n')//用这个代表换行
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
			
		putfonts8_asc_sht(sht_back, *nPos_CurX, *nPos_CurY,COL_BLACK,COL_WHITE, szChar, 1);//在图层上显示文字
		//nTextSel++;		//取字符 位置加1					
		*nPos_CurX+=8;	//光标移至下一个字符位置
						
		if(*nPos_CurX>nXSize-20)//超过一行
		{
			if(*nPos_CurY<nYSize-40)//换行后不会超过底部
			{
				*nPos_CurX=12;
				*nPos_CurY+=18;
				//putfonts8_asc_sht(sht_back, *nPos_CurX, *nPos_CurY,COL_BLACK,COL_WHITE, szChar, 1);//在图层上显示文字
			}
		}									
	}
}

/*退格功能*/
void TextCurBack(struct SHEET *sht_back,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY)
{
	if(*nPos_CurX>15)
	{
		*nPos_CurX-=8;	//光标移退一格
	}
	else if(*nPos_CurY>30)//退到上一行
	{
		RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, *nPos_CurX, *nPos_CurY, *nPos_CurX+1 , *nPos_CurY + 16);
		sheet_refresh(sht_back, *nPos_CurX, *nPos_CurY, *nPos_CurX+10+1 , *nPos_CurY +16+1);
		*nPos_CurX=428;
		*nPos_CurY-=18;
	}
	/*覆盖块 白色*/
	RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, *nPos_CurX, *nPos_CurY, *nPos_CurX+10 , *nPos_CurY + 16);
	sheet_refresh(sht_back, *nPos_CurX, *nPos_CurY, *nPos_CurX+10+1 , *nPos_CurY +16+1);
	
}

/* Window任务*/
void task_win_main(struct SHEET *sht_back)
{
	struct FIFO32 MsgFifo;//任务消息队列
	struct TIMER *timer_text_Cur;//光标闪烁定时器
	struct TIMER *timer_put;//介绍文字输出定时器
	struct TASK *task = task_now();//当前任务，即本身
	task->fifo=&MsgFifo;//设置任务的fifo缓冲区
	
	int nXSize=450;//窗体宽度
	int nYSize=300;//窗体高度

	int nCurTime=30;//光标闪烁间隔
	int nPutTime=3;//介绍文字输出间隔
	
	int MsgFifo_buf[128];
	int s[10];
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
	//-------------------------------------------------------------------------------------
			else if (256 <= i && i <= 511)/* 任务A传送过来的键盘的数据 */ 				
			{ 
				i-=256;
				#if TEST_ING
				sprintf(s, "%02d", i);
				putfonts8_asc_sht(sht_back, 0, 0,COL_BLACK,COL_WHITE, s, 4);//在图层上显示文字
				#endif
				
				if (i == 8)/* 退格键 */ 
				{		
					TextCurBack(sht_back,nXSize,nYSize,&nPos_CurX,&nPos_CurY);
					
				}
				else if(i ==7)//换行
				{
					szChar[0]='\n';
					szChar[1]=0;
					PutWinTextChar(sht_back,szChar,nXSize,nYSize,&nPos_CurX,&nPos_CurY);
				}
				else /* 一般字符 ,在任务A中已经将键值转换成字符的ASCII码*/
				{
					szChar[0]=i;
					szChar[1]=0;
					PutWinTextChar(sht_back,szChar,nXSize,nYSize,&nPos_CurX,&nPos_CurY);	
					
				}
			}
	//-------------------------------------------------------------------------------------
			
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