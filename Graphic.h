#ifndef _GRAPHIC_H
#define _GRAPHIC_H


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

#endif
