#include "common.h"
/* ��ʼ����ɫ�� */
void init_palette(void)
{
	/* static�����ı�����"��̬"��  �ڳ������֮ǰһֱ"����" */
	static unsigned char table_rgb[16 * 3] = 
	{
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���� */
		0x00, 0xff, 0x00,	/*  2:���� */
		0xff, 0xff, 0x00,	/*  3:���� */
		0x00, 0x00, 0xff,	/*  4:���� */
		0xff, 0x00, 0xff,	/*  5:���� */
		0x00, 0xff, 0xff,	/*  6:ǳ���� */
		0xff, 0xff, 0xff,	/*  7:�� */
		0xc6, 0xc6, 0xc6,	/*  8:���� */
		0x84, 0x00, 0x00,	/*  9:���� */
		0x00, 0x84, 0x00,	/* 10:���� */
		0x84, 0x84, 0x00,	/* 11:���� */
		0x00, 0x00, 0x84,	/* 12:���� */
		0x84, 0x00, 0x84,	/* 13:���� */
		0x60, 0xA0, 0xC0,	/* 14:ǳ���� */
		0x84, 0x84, 0x84	/* 15:���� */
	};
	set_palette(0, 15, table_rgb);		/* ���õ�ɫ�� */
	return;
}

/* ���õ�ɫ�庯�� */
void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* ����eflags��ֵ */
	io_cli(); 					/* �ر����п������ж� */
	io_out8(0x03c8, start);	
/*	
	for (i = start; i <= end; i++) 
	{
		io_out8(0x03c9, rgb[0]/4);
		io_out8(0x03c9, rgb[1]/4);
		io_out8(0x03c9, rgb[2]/4);
		rgb += 3;
	}
*/	
	unsigned char *p=0x10B036;
	unsigned char *pTemp=0x100000;
	
	for(i=0;i<=255;i++)
	{
		
	/*	if(i==0)
		{
	
		io_out8(0x03c9, 0x60/4);
		io_out8(0x03c9, 0xa0/4);
		io_out8(0x03c9, 0xc0/4);
		
		*pTemp=*p;
		*(pTemp+1)=*(p+1);
		*(pTemp+2)=*(p+2);
		}
		else
		{*/
	
		io_out8(0x03c9, *(p+2)/4);
		io_out8(0x03c9, *(p+1)/4);
		io_out8(0x03c9, *p/4);
	//	}
		p +=4;
	}
	
	io_store_eflags(eflags);	/* �ָ�eflags��ֵ */
	return;
}


/*���һ������*/
void RectFill(unsigned char *vram, int nXSize,unsigned char Color, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) 
	{
		for (x = x0; x <= x1; x++)
		{
			vram[y * nXSize + x] = Color;
		}
	}
	return;
}

/*���Ʊ���*/
void DrawBack(char *vram, int nXSize, int nYSize)
{
	RectFill(vram,nXSize,0xeb,0,0,nXSize,nYSize);//����
	
	struct tagBITMAPFILEHEADER  *BmpHead= (struct tagBITMAPFILEHEADER *) 0x10AE02;//��ȡ����ʱ�򱣴����Ϣ
	
	char szTemp[30];
	unsigned char *pTemp;
	pTemp=&(BmpHead->bfSize);
	//pTemp=(unsigned char *)0x10AE00;
	sprintf(szTemp, "Bmp:%x",BmpHead->bfSize);
	PutFont_Asc(vram, nXSize, nXSize-260, nYSize-23, COL_WHITE, szTemp); // ��ʾ������Ϣ 
	
	int i,j;
	
	unsigned char *p=0x10B236;//����ͼƬ�ڴ��ַ
	
	int nXOffset=288;
	for(i=599;i>=0;i--)
	{
		for(j=0;j<800;j++)
		{
			/*
			if(*p==0xff)
			{
				vram[i * nXSize + j + nXOffset] =COL_WHITE;
			}
			else
			{*/
				vram[i * nXSize + j + nXOffset] =*p;
			//}
			p++;
		}
	}
	
	
	RectFill(vram,nXSize,COL_BLACK,0,nYSize-30,nXSize,nYSize);//������
	RectFill(vram,nXSize,COL_DARK_GREY,43,nYSize-25,43,nYSize-5);//�����������
	
	PutFont_Asc(vram, nXSize, nXSize+13, nYSize-23,COL_GREEN, "SML"); // ���ͼ��
	PutFont_Asc(vram, nXSize, nXSize+14, nYSize-23,COL_GREEN, "SML");
	
	RectFill(vram,nXSize,COL_DARK_GREY,nXSize-70,nYSize-25,nXSize-70,nYSize-5);//�������ұ���
	
	PutFont_Asc(vram, nXSize, nXSize-60, nYSize-23, COL_WHITE, "000 Sec"); // ��ʾ������Ϣ 
	
	//p=0x100000;
	sprintf(szTemp, "Bmp:%x,%x,%x",*p,*(p+1),*(p+2));
	//RectFill(vram,nXSize,COL_BLACK,nXSize-260,nYSize-23,nXSize,nYSize);//������
	//PutFont_Asc(vram, nXSize, nXSize-260, nYSize-23, COL_WHITE, szTemp); // ��ʾ������Ϣ 
	/**/
}

/*����ַ���*/
void PutFont_Asc(unsigned char *vram, int nXSize, int x, int y, char c,unsigned char *s)
{
	extern char Font[4096];
	
	for (; *s != 0x00; s++) 
	{
		PutFont8(vram, nXSize, x, y, c, Font + *s * 16);
		x += 8;
	}
	return;
}
/*��������ַ�*/
void PutFont8(unsigned char *vram, int nXSize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) 
	{
		p = vram + (y + i) * nXSize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

/*��ʼ�� ���ͼ������*/
void Init_MouseCur(char *mouse, char cBackColor)
{
	static char cursor[16][16] = 
	{
		"**..............",
		"*O*.............",
		"*OO*............",
		"*OOO*...........",
		"*OOOO*..........",
		"*OOOOO*.........",
		"*OOOOOO*........",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOOOOOOO*.....",
		"*OOOO******.....",
		"*OOO*...........",
		"*OO*............",
		"*O*.............",
		"**..............",
		"................"
	};
	int x, y;

	for (y = 0; y < 16; y++) 
	{
		for (x = 0; x < 16; x++) 
		{
			if (cursor[y][x] == '*') 
			{
				mouse[y * 16 + x] = COL_DARK_GREEN;
			}
			if (cursor[y][x] == 'O') 
			{
				//mouse[y * 16 + x] = cBackColor;
				mouse[y * 16 + x] = COL_WHITE;
			}
			if (cursor[y][x] == '.') 
			{
				mouse[y * 16 + x] = cBackColor;
			}
		}
	}
	return;
}

/*��ʾһ��ͼ���*/
void PutBlock(unsigned char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) 
	{
		for (x = 0; x < pxsize; x++) 
		{
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

/* ��������ͼ�� */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title)
{
	/* ������鶨����Ǵ������Ͻ��Ǹ����ͼ�� */
	static char closebtn[13][20] = 
	{
		"QQQQQQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQQQQQ",
		"QQQQQQ@@QQQQ@@QQQQQQ",
		"QQQQQQQ@@QQ@@QQQQQQQ",
		"QQQQQQQQ@@@@QQQQQQQQ",
		"QQQQQQQQQ@@QQQQQQQQQ",
		"QQQQQQQQ@@@@QQQQQQQQ",
		"QQQQQQQ@@QQ@@QQQQQQQ",
		"QQQQQQ@@QQQQ@@QQQQQQ",
		"QQQQQQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQQQQQ"
	};
	int x, y;
	char c;
	
	/* �Ȼ������ڵ���״ */
	RectFill(buf, xsize, COL_BLACK, 0,	0,xsize - 1, ysize-1);
	RectFill(buf, xsize, COL_GREY, 3,	22,xsize - 4, ysize-4);
	
	//RectFill(buf, xsize, COL_GREY, 0,	0,0, ysize-1);
	//RectFill(buf, xsize, COL_GREY, 0,	0,xsize - 1, 0);
	//RectFill(buf, xsize, COL_GREY, xsize - 1,	0,xsize - 1, ysize-1);
	//RectFill(buf, xsize, COL_GREY, 0,	ysize-1,xsize - 1, ysize-2);
	
	/* ������ڵı��� */
	PutFont_Asc(buf, xsize, 20, 3, COL_WHITE, title);
	/* �������ϽǵĲ�� */
	for (y = 0; y < 13; y++) 
	{
		for (x = 0; x < 20; x++) 
		{
			c = closebtn[y][x];
			if (c == '@') 
			{
				c = COL_WHITE;
			} 
			else if (c == 'Q') 
			{
				c = COL_RED;
			} 

			buf[(4 + y) * xsize + (xsize - 25 + x)] = c;
		}
	}
	return;
}

/* �������ڵ��ı��� */
void make_window_edit(unsigned char *buf, int xsize, int ysize)
{
	int nX1=8;
	int nY1=26;
	int nX2=xsize - 8;
	int nY2=ysize - 8;

	RectFill(buf, xsize, COL_WHITE,nX1,nY1,nX2,nY2);
	/*���ı��� �߿�*/
	RectFill(buf, xsize, COL_DARK_GREY, nX1,nY1,nX2,nY1);
	RectFill(buf, xsize, COL_DARK_GREY, nX1,nY1,nX1,nY2);
	RectFill(buf, xsize, COL_DARK_GREY, nX1,nY2,nX2,nY2);
	RectFill(buf, xsize, COL_DARK_GREY, nX2,nY1,nX2,nY2);
	
	return;
}

/* 
������ֵ�ָ��ͼ�㣺
	sht �ṹ��ͼ��
	x,y	��ʾλ�õ����Ͻ�����
	c	�ַ���ɫ
	b	������ɫ
	s	�ַ���
	l	�ַ�������
 */
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l)
{

	RectFill(sht->buf, sht->bxsize, b, x, y, x + l * 8 , y + 16);
	PutFont_Asc(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8+1, y + 16+1);//ˢ�º��Ͼ���һ���󣬼���1
	return;
}
