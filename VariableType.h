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

/*-----FIFO缓冲区结构体 --------------------------------*/
struct FIFO8
{						
	unsigned char *buf;	/* 缓冲区指针 */
	int p;				/* p 下一个数据写入位置*/
	int q; 				/*q 下一个数据读出位置*/
	int size; 			/*size 缓冲区的总字节数	*/
	int free; 			/*free 缓冲区的空闲字节数*/
	int flags;			/*flags	记录缓冲区是否溢出*/
};

/* 32bit的队列缓冲区结构体 */
struct FIFO32 
{
	int *buf;						/* 缓冲区指针 */
	int p;				/* p 下一个数据写入位置*/
	int q; 				/*q 下一个数据读出位置*/
	int size; 			/*size 缓冲区的总字节数	*/
	int free; 			/*free 缓冲区的空闲字节数*/
	int flags;			/*flags	记录缓冲区是否溢出*/
	struct TASK *task;	/* 当队列缓冲区写入数据时需要唤醒的任务 */
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
	struct FIFO32 *fifo;
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
#define MAX_TASKS_LV	100		/* 每个LEVEL最多允许建立100个任务 */
#define MAX_TASKLEVELS	10		/* 最多允许建立10个LEVEL */

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
   level	任务运行运行层上(越低层优先运行)
   priority	该任务的优先级（运行时间-10ms单位）
   tss		TSS数据结构 用于任务切换时保存任务的寄存器及任务的配置信息
   fifo		该任务缓冲区
*/
struct TASK
{
	int sel, flags; 
	int level;
	int priority;
	struct TSS32 tss;
	struct FIFO32 *fifo;
};

/* 任务的分层结构*/
/* 
   running	该层中有多少个任务在运行
   now		该层中正在运行的是哪个任务
   tasks	TASK的指针数组,和sheets类似,数组中的元素按照顺序排放
 */
struct TASKLEVEL 
{
	int running; 
	int now; 
	struct TASK *tasks[MAX_TASKS_LV];
};

/* 管理所有任务的结构体 */
/* 
   now_lv	当前任务运行在哪层上（lv==level）
   lv_change	下次任务切换时,是否需要修改level
   level[]	所有的level都定义在该数组中
   tasks0	注意！系统中的任务虽然分属不同的层，但是任何标识一个任务的结构体都是在这个数组中的
 */
struct TASKCTL 
{
	int now_lv; 
	char lv_change; 
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

/*----窗体任务信息------------------------------------------*/
struct WINDOW_INFO
{
	int nXSize,nYSize;
	char title[20];
	struct TASK task;
};
/*----解析位图相关------------------------------------------*/
typedef unsigned char BYTE;  
typedef unsigned short WORD;  
typedef unsigned int DWORD;  
typedef int LONG;  
  
  
//位图文件头信息结构定义  
//其中不包含文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确读取文件信息）  
  
typedef struct tagBITMAPFILEHEADER 
{  
//WORD bfType;//固定为0x4d42;
DWORD bfSize;           //文件大小  
WORD   bfReserved1;     //保留字，不考虑  
WORD   bfReserved2;     //保留字，同上  
DWORD bfOffBits;        //实际位图数据的偏移字节数，即前三个部分长度之和  
} BITMAPFILEHEADER;   
  
  
//信息头BITMAPINFOHEADER，也是一个结构，其定义如下：  
  
typedef struct tagBITMAPINFOHEADER
{  
//public:  
DWORD   biSize;             //指定此结构体的长度，为40  
LONG    biWidth;            //位图宽  
LONG    biHeight;           //位图高  
WORD    biPlanes;           //平面数，为1  
WORD    biBitCount;         //采用颜色位数，可以是1，2，4，8，16，24，新的可以是32  
DWORD   biCompression;      //压缩方式，可以是0，1，2，其中0表示不压缩  
DWORD   biSizeImage;        //实际位图数据占用的字节数  
LONG    biXPelsPerMeter;    //X方向分辨率  
LONG    biYPelsPerMeter;    //Y方向分辨率  
DWORD   biClrUsed;          //使用的颜色数，如果为0，则表示默认值(2^颜色位数)  
DWORD   biClrImportant;     //重要颜色数，如果为0，则表示所有颜色都是重要的  
} BITMAPINFOHEADER;   
  
  
//调色板Palette，当然，这里是对那些需要调色板的位图文件而言的。24位和32位是不需要调色板的。  
//（似乎是调色板结构体个数等于使用的颜色数。）  
  
typedef struct tagRGBQUAD 
{   
//public:  
BYTE     rgbBlue; //该颜色的蓝色分量  
BYTE     rgbGreen; //该颜色的绿色分量  
BYTE     rgbRed; //该颜色的红色分量  
BYTE     rgbReserved; //保留值  
} RGBQUAD;  
  

#endif
