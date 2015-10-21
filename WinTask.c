#include "common.h"

/*-----Ӧ������---------------------------------------*/

/*���һ���ַ��������ı���*/
void PutWinTextChar(struct SHEET *sht_back,char *s,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY)
{
	char szChar[2];//��ȡ�����ַ�����ʱ����
	szChar[0]=*s;
	szChar[1]=0;
					
	if(szChar[0]=='\n')//�����������
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
	
	if(*nPos_CurX<nXSize-20)//��һ���ڿ������
	{				
			
		putfonts8_asc_sht(sht_back, *nPos_CurX, *nPos_CurY,COL_BLACK,COL_WHITE, szChar, 1);//��ͼ������ʾ����
		//nTextSel++;		//ȡ�ַ� λ�ü�1					
		*nPos_CurX+=8;	//���������һ���ַ�λ��
						
		if(*nPos_CurX>nXSize-20)//����һ��
		{
			if(*nPos_CurY<nYSize-40)//���к󲻻ᳬ���ײ�
			{
				*nPos_CurX=12;
				*nPos_CurY+=18;
				//putfonts8_asc_sht(sht_back, *nPos_CurX, *nPos_CurY,COL_BLACK,COL_WHITE, szChar, 1);//��ͼ������ʾ����
			}
		}									
	}
}

/*�˸���*/
void TextCurBack(struct SHEET *sht_back,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY)
{
	if(*nPos_CurX>15)
	{
		*nPos_CurX-=8;	//�������һ��
	}
	else if(*nPos_CurY>30)//�˵���һ��
	{
		RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, *nPos_CurX, *nPos_CurY, *nPos_CurX+1 , *nPos_CurY + 16);
		sheet_refresh(sht_back, *nPos_CurX, *nPos_CurY, *nPos_CurX+10+1 , *nPos_CurY +16+1);
		*nPos_CurX=428;
		*nPos_CurY-=18;
	}
	/*���ǿ� ��ɫ*/
	RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, *nPos_CurX, *nPos_CurY, *nPos_CurX+10 , *nPos_CurY + 16);
	sheet_refresh(sht_back, *nPos_CurX, *nPos_CurY, *nPos_CurX+10+1 , *nPos_CurY +16+1);
	
}

/* Window����*/
void task_win_main(struct SHEET *sht_back)
{
	struct FIFO32 MsgFifo;//������Ϣ����
	struct TIMER *timer_text_Cur;//�����˸��ʱ��
	struct TIMER *timer_put;//�������������ʱ��
	struct TASK *task = task_now();//��ǰ���񣬼�����
	task->fifo=&MsgFifo;//���������fifo������
	
	int nXSize=450;//������
	int nYSize=300;//����߶�

	int nCurTime=30;//�����˸���
	int nPutTime=3;//��������������
	
	int MsgFifo_buf[128];
	int s[10];
	int i;
		
	int nPos_CurX=12;
	int nPos_CurY=30;
	
	char szChar[2];//��ȡ�����ַ�����ʱ����
	int nTextSel=0,nTextLine=0;//�������������λ��,����
	
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
			task_sleep(task);					/* ˯���Լ� */
			io_sti();
		} 
		else 
		{
			
			i = fifo32_get(&MsgFifo);
			io_sti();//���ж�
			
			if (i == 1)//��궨ʱ��
			{
					/*������ ��ɫ*/
				RectFill(sht_back->buf, sht_back->bxsize, COL_PURPLE, nPos_CurX, nPos_CurY, nPos_CurX+1 , nPos_CurY + 16);
				sheet_refresh(sht_back, nPos_CurX, nPos_CurY, nPos_CurX+10+1 , nPos_CurY + 16+1);
				timer_init(timer_text_Cur, &MsgFifo, 0);
				timer_settime(timer_text_Cur, nCurTime);	

			}
			else if(i==0)//��궨ʱ��
			{
				/*������ ��ɫ*/
				RectFill(sht_back->buf, sht_back->bxsize, COL_WHITE, nPos_CurX, nPos_CurY, nPos_CurX+1 , nPos_CurY + 16);
				sheet_refresh(sht_back, nPos_CurX, nPos_CurY, nPos_CurX+10+1 , nPos_CurY +16+1);
				timer_init(timer_text_Cur, &MsgFifo, 1);
				timer_settime(timer_text_Cur, nCurTime);
			}
			else if((i==2))//���������ʱ��
			{
				if(nTextLine<5)//�ַ�����
				{
					szChar[0]=*(szText[nTextLine]+nTextSel);
					szChar[1]=0;
					
					if(szChar[0]!='\0')
					{
						PutWinTextChar(sht_back,szChar,nXSize,nYSize,&nPos_CurX,&nPos_CurY);//�������
						nTextSel++;
					}
					else
					{
						//����
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
			else if (256 <= i && i <= 511)/* ����A���͹����ļ��̵����� */ 				
			{ 
				i-=256;
				#if TEST_ING
				sprintf(s, "%02d", i);
				putfonts8_asc_sht(sht_back, 0, 0,COL_BLACK,COL_WHITE, s, 4);//��ͼ������ʾ����
				#endif
				
				if (i == 8)/* �˸�� */ 
				{		
					TextCurBack(sht_back,nXSize,nYSize,&nPos_CurX,&nPos_CurY);
					
				}
				else if(i ==7)//����
				{
					szChar[0]='\n';
					szChar[1]=0;
					PutWinTextChar(sht_back,szChar,nXSize,nYSize,&nPos_CurX,&nPos_CurY);
				}
				else /* һ���ַ� ,������A���Ѿ�����ֵת�����ַ���ASCII��*/
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
/* ����B(Task1)*/
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
	
	putfonts8_asc_sht(sht_back, 22, 28,COL_BLACK,COL_APPLE_GRREN, " Priority:1ms", 16);//��ͼ������ʾ����
	
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
				putfonts8_asc_sht(sht_back, 22, 50,COL_BLACK,COL_APPLE_GRREN, s, 16);//��ͼ������ʾ����
				
				timer_settime(timer_put, 10);		
			}
		}
	}
	
}
/* ����B(Task2)*/
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
	
	putfonts8_asc_sht(sht_back, 22, 28,COL_BLACK,COL_APPLE_GRREN, " Priority:2ms", 16);//��ͼ������ʾ����
	
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
				putfonts8_asc_sht(sht_back, 22, 50,COL_BLACK,COL_APPLE_GRREN, s, 16);//��ͼ������ʾ����
				 
				timer_settime(timer_put, 10);		
			}
		}
	}
	
}