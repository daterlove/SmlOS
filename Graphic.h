#ifndef _GRAPHIC_H
#define _GRAPHIC_H

/* 初始化调色板 */
void init_palette(void);
/* 设置调色板函数 */
void set_palette(int start, int end, unsigned char *rgb);
/*填充一个矩形*/
void RectFill(unsigned char *vram, int nXSize,unsigned char Color, int x0, int y0, int x1, int y1);
/*绘制背景*/
void DrawBack(char *vram, int nXSize, int nYSize);
/*输出字符串*/
void PutFont_Asc(unsigned char *vram, int nXSize, int x, int y, char c,unsigned char *s);
/*输出单个字符*/
void PutFont8(unsigned char *vram, int nXSize, int x, int y, char c, char *font);
/*初始化 鼠标图像数组*/
void Init_MouseCur(char *mouse, char cBackColor);
/*显示一个图像块*/
void PutBlock(unsigned char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, char *buf, int bxsize);
/* 创建窗口图形 */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title);
/* 创建窗口的文本框 */
void make_window_edit(unsigned char *buf, int xsize, int ysize);
/* 输出文字到指定图层*/
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);

#endif
