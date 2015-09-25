#ifndef _VARIABLETYPE_H
#define _VARIABLETYPE_H

/*启动区信息 结构体*/
struct BOOTINFO 
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

/* 鼠标缓冲区结构 
	buf[]是缓冲区,存放鼠标发送过来的3字节数据
	phase用来标识接收到鼠标数据的第几步
	x, y存放鼠标位移量	bth记录按键信息
 */
 struct MOUSE_DEC 
{
	unsigned char buf[3], phase;
	int x, y, btn;
};

/*---------------------------------------------------------------*/
#define MEMMAN_FREES		4000	/* FREEINFO结构的数量*/

/* 内存可用信息条目 
 addr 可用内存块首地址，size 可用内存块大小 */
struct FREEINFO 
{
	unsigned int addr, size;
};

/* 内存管理结构 
	frees 内存可用信息条目的数目
	maxfrees	frees的最大值
	lostsize	释放失败的内存大小的总和
	losts		释放失败的次数
	struct FREEINFO free[MEMMAN_FREES];	内存可用信息条目数组
 */
struct MEMMAN
{		
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

/*---------------------------------------------------------------*/
#define MAX_SHEETS		256			/* 图层的最大数 */
/* 图层结构体 */
/*
   用来记录单个图层的信息
   buf	是记录图层上所描画内容的地址
   bxsize, bysize	图层的行数和列数
   vx0, vy0			图层左上角的坐标
   col_inv			图层的透明色色号 (所谓的图层的透明色:以鼠标图层为例,在HariMain函数中分别有以下两条调用语句
									  sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);		设置鼠标图层信息
									  init_mouse_cursor8(buf_mouse, 99);		 			初始化鼠标图形 
									  sheet_setbuf的最后一个参数99就是设置了鼠标图层的col_inv成员变量 即透明色
									  而init_mouse_cursor8中的参数99设置了鼠标图形中需显示背景的部分的色号,具体
									  请参考该函数的定义,在描绘鼠标图形的矩形中不是鼠标形状的部分即使用该色号
									  也就是说凡是色号为99的部分都不需要重新描绘)
   height			图层的高度
   flags			记录图层各种状态信息
 */
struct SHEET 
{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
};
/* 管理各个图层的结构体 */
/*
   vram		图像缓冲区首地址	
   xsize, ysize		分辨率的x和y	与BOOTINFO中的值相同
   sheets	图层指针数组, 指向sheets0中各个图层
   sheets0	图层数组, 存放各个图层结构
 */
struct SHTCTL 
{
	unsigned char *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
#endif
