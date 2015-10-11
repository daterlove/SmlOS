#ifndef _GRAPHIC_H
#define _GRAPHIC_H

/* ��ʼ����ɫ�� */
void init_palette(void);
/* ���õ�ɫ�庯�� */
void set_palette(int start, int end, unsigned char *rgb);
/*���һ������*/
void RectFill(unsigned char *vram, int nXSize,unsigned char Color, int x0, int y0, int x1, int y1);
/*���Ʊ���*/
void DrawBack(char *vram, int nXSize, int nYSize);
/*����ַ���*/
void PutFont_Asc(unsigned char *vram, int nXSize, int x, int y, char c,unsigned char *s);
/*��������ַ�*/
void PutFont8(unsigned char *vram, int nXSize, int x, int y, char c, char *font);
/*��ʼ�� ���ͼ������*/
void Init_MouseCur(char *mouse, char cBackColor);
/*��ʾһ��ͼ���*/
void PutBlock(unsigned char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, char *buf, int bxsize);
/* ��������ͼ�� */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title);
/* �������ڵ��ı��� */
void make_window_edit(unsigned char *buf, int xsize, int ysize);
/* ������ֵ�ָ��ͼ��*/
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);

#endif
