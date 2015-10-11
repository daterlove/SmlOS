#ifndef _VARIABLETYPE_H
#define _VARIABLETYPE_H

/*-----启动信息及鼠标结构体--------------------------------*/
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

/*-----内存管理相关------------------------------------------*/
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

/*----图层管理相关------------------------------------------*/
#define MAX_SHEETS		256			/* 图层的最大数 */
/* 图层结构体 */
/*
   用来记录单个图层的信息
   buf	是记录图层上所描画内容的地址
   bxsize, bysize	图层的行数和列数
   vx0, vy0			图层左上角的坐标
   col_inv			图层的透明色色号 
   height			图层的高度
   flags			记录图层各种状态信息
 */
struct SHEET 
{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
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
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *p_sheets[MAX_SHEETS];
	struct SHEET sheets[MAX_SHEETS];
};

/*-----时钟控制相关-----------------------------------------*/
#define MAX_TIMER		500			/* 定时器最多可以有500个 */

/* 定时器结构体 */
/* 
   timeout	相对于当前时间的预定时刻
   flags	记录定时器状态
   fifo		定时器队列指针
   data		定时器数据
 */
struct TIMER
{
	unsigned int timeout, flags;
	struct FIFO8 *fifo;
	unsigned char data;
};

/* 管理定时器的结构 */
/* 
   count	计数器		next	"下一时刻"(不知道怎么描述..)
   using	当前活动的定时器数量
   timers	定时器结构指针数组	(注意：这是一个有序数组,和sheets[]结构是一样的作用)
   timers0	定时器结构数组
 */
struct TIMERCTL
{
	unsigned int count, next, using;
	struct TIMER *timers[MAX_TIMER];
	struct TIMER timers0[MAX_TIMER];
};

/*----任务切换相关------------------------------------------*/
#define MAX_TASKS		1000	/* 最大任务数 */
#define TASK_GDT0		3		/* TSS描述符从GDT中下标为3处开始 */


/* TSS结构 （Task-State Segment)*/
struct TSS32 
{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

/* 标识一个任务的数据结构 */
/* 
   sel		该任务的段选择子(GDT中的选择子)
   flag		标识该任务的状态
   priority	该任务的优先级
   tss		TSS数据结构 用于任务切换时保存任务的寄存器及任务的配置信息
*/
struct TASK
{
	int sel, flags; 
	int priority;
	struct TSS32 tss;
};

/* 管理所有任务的结构体 */
/* 
   running 	运行中任务个数
   now		正在运行的任务
*/
struct TASKCTL 
{
	int running; 
	int now;
	struct TASK *tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
};
/*----窗体信息------------------------------------------*/
struct WINDOW_INFO
{
	int nXSize,nYSize;
	char title[20];
};
#endif
