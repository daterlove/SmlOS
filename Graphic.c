#include "common.h"

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
