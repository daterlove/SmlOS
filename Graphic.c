#include "common.h"
/* ��ʼ����ɫ�� */
void init_palette(void)
{
	/* static�����ı�����"��̬"��  �ڳ������֮ǰһֱ"����" */
	static unsigned char table_rgb[16 * 3] = {
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
		0x00, 0x84, 0x84,	/* 14:ǳ���� */
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
	for (i = start; i <= end; i++) 
	{
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
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
	RectFill(vram,nXSize,COL_BACK_BLUE,0,0,nXSize,nYSize);
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
				mouse[y * 16 + x] = COL_BLACK;
			}
			if (cursor[y][x] == 'O') 
			{
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
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	/* �Ȼ������ڵ���״ */
	/*
	RectFill(buf, xsize, COL_DARK_GREY, 0,         0,         xsize - 1, 0        );
	RectFill(buf, xsize, COL_BLUE, 1,         1,         xsize - 2, 1        );
	RectFill(buf, xsize, COL_DARK_GREY, 0,         0,         0,         ysize - 1);
	RectFill(buf, xsize, COL_BLUE, 1,         1,         1,         ysize - 2);
	RectFill(buf, xsize, COL_GREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	RectFill(buf, xsize, COL_BLACK, xsize - 1, 0,         xsize - 1, ysize - 1);
	RectFill(buf, xsize, COL_GREEN, 2,         2,         xsize - 3, ysize - 3);
	RectFill(buf, xsize, COL_DARK_GREY, 3,         3,         xsize - 4, 20       );
	RectFill(buf, xsize, COL_GREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	RectFill(buf, xsize, COL_BLACK, 0,         ysize - 1, xsize - 1, ysize - 1);
	*/
	RectFill(buf, xsize, COL_BLACK, 0,	0,xsize - 1, ysize-1);
	RectFill(buf, xsize, COL_GREY, 2,	20,xsize - 3, ysize-3);
	
	/* ������ڵı��� */
	PutFont_Asc(buf, xsize, 20, 3, COL_WHITE, title);
	/* �������ϽǵĲ�� */
	for (y = 0; y < 14; y++) 
	{
		for (x = 0; x < 16; x++) 
		{
			c = closebtn[y][x];
			if (c == '@') 
			{
				c = COL_BLACK;
			} 
			else if (c == '$') 
			{
				c = COL_RED;
			} 
			else if (c == 'Q') 
			{
				c = COL_RED;
			} 
			else 
			{
				c = COL_RED;
			}
			buf[(3 + y) * xsize + (xsize - 19 + x)] = c;
		}
	}
	return;
}
