#ifndef _VARIABLETYPE_H
#define _VARIABLETYPE_H

/*-----������Ϣ�����ṹ��--------------------------------*/
/*��������Ϣ �ṹ��*/
struct BOOTINFO 
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

/* ��껺�����ṹ 
	buf[]�ǻ�����,�����귢�͹�����3�ֽ�����
	phase������ʶ���յ�������ݵĵڼ���
	x, y������λ����	bth��¼������Ϣ
 */
 struct MOUSE_DEC 
{
	unsigned char buf[3], phase;
	int x, y, btn;
};

/*-----FIFO�������ṹ�� --------------------------------*/
struct FIFO8
{						
	unsigned char *buf;	/* ������ָ�� */
	int p;				/* p ��һ������д��λ��*/
	int q; 				/*q ��һ�����ݶ���λ��*/
	int size; 			/*size �����������ֽ���	*/
	int free; 			/*free �������Ŀ����ֽ���*/
	int flags;			/*flags	��¼�������Ƿ����*/
};

/* 32bit�Ķ��л������ṹ�� */
struct FIFO32 
{
	int *buf;						/* ������ָ�� */
	int p;				/* p ��һ������д��λ��*/
	int q; 				/*q ��һ�����ݶ���λ��*/
	int size; 			/*size �����������ֽ���	*/
	int free; 			/*free �������Ŀ����ֽ���*/
	int flags;			/*flags	��¼�������Ƿ����*/
	struct TASK *task;	/* �����л�����д������ʱ��Ҫ���ѵ����� */
};

/*-----�ڴ�������------------------------------------------*/
#define MEMMAN_FREES		4000	/* FREEINFO�ṹ������*/

/* �ڴ������Ϣ��Ŀ 
 addr �����ڴ���׵�ַ��size �����ڴ���С */
struct FREEINFO 
{
	unsigned int addr, size;
};

/* �ڴ����ṹ 
	frees �ڴ������Ϣ��Ŀ����Ŀ
	maxfrees	frees�����ֵ
	lostsize	�ͷ�ʧ�ܵ��ڴ��С���ܺ�
	losts		�ͷ�ʧ�ܵĴ���
	struct FREEINFO free[MEMMAN_FREES];	�ڴ������Ϣ��Ŀ����
 */
struct MEMMAN
{		
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

/*----ͼ��������------------------------------------------*/
#define MAX_SHEETS		256			/* ͼ�������� */
/* ͼ��ṹ�� */
/*
   ������¼����ͼ�����Ϣ
   buf	�Ǽ�¼ͼ�������軭���ݵĵ�ַ
   bxsize, bysize	ͼ�������������
   vx0, vy0			ͼ�����Ͻǵ�����
   col_inv			ͼ���͸��ɫɫ�� 
   height			ͼ��ĸ߶�
   flags			��¼ͼ�����״̬��Ϣ
 */
struct SHEET 
{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
};
/* �������ͼ��Ľṹ�� */
/*
   vram		ͼ�񻺳����׵�ַ	
   xsize, ysize		�ֱ��ʵ�x��y	��BOOTINFO�е�ֵ��ͬ
   sheets	ͼ��ָ������, ָ��sheets0�и���ͼ��
   sheets0	ͼ������, ��Ÿ���ͼ��ṹ
 */
struct SHTCTL 
{
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *p_sheets[MAX_SHEETS];
	struct SHEET sheets[MAX_SHEETS];
};

/*-----ʱ�ӿ������-----------------------------------------*/
#define MAX_TIMER		500			/* ��ʱ����������500�� */

/* ��ʱ���ṹ�� */
/* 
   timeout	����ڵ�ǰʱ���Ԥ��ʱ��
   flags	��¼��ʱ��״̬
   fifo		��ʱ������ָ��
   data		��ʱ������
 */
struct TIMER
{
	unsigned int timeout, flags;
	struct FIFO32 *fifo;
	unsigned char data;
};

/* ����ʱ���Ľṹ */
/* 
   count	������		next	"��һʱ��"(��֪����ô����..)
   using	��ǰ��Ķ�ʱ������
   timers	��ʱ���ṹָ������	(ע�⣺����һ����������,��sheets[]�ṹ��һ��������)
   timers0	��ʱ���ṹ����
 */
struct TIMERCTL
{
	unsigned int count, next, using;
	struct TIMER *timers[MAX_TIMER];
	struct TIMER timers0[MAX_TIMER];
};

/*----�����л����------------------------------------------*/
#define MAX_TASKS		1000	/* ��������� */
#define TASK_GDT0		3		/* TSS��������GDT���±�Ϊ3����ʼ */
#define MAX_TASKS_LV	100		/* ÿ��LEVEL���������100������ */
#define MAX_TASKLEVELS	10		/* ���������10��LEVEL */

/* TSS�ṹ ��Task-State Segment)*/
struct TSS32 
{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

/* ��ʶһ����������ݽṹ */
/* 
   sel		������Ķ�ѡ����(GDT�е�ѡ����)
   flag		��ʶ�������״̬
   level	�����������в���(Խ�Ͳ���������)
   priority	����������ȼ�������ʱ��-10ms��λ��
   tss		TSS���ݽṹ ���������л�ʱ��������ļĴ����������������Ϣ
   fifo		�����񻺳���
*/
struct TASK
{
	int sel, flags; 
	int level;
	int priority;
	struct TSS32 tss;
	struct FIFO32 *fifo;
};

/* ����ķֲ�ṹ*/
/* 
   running	�ò����ж��ٸ�����������
   now		�ò����������е����ĸ�����
   tasks	TASK��ָ������,��sheets����,�����е�Ԫ�ذ���˳���ŷ�
 */
struct TASKLEVEL 
{
	int running; 
	int now; 
	struct TASK *tasks[MAX_TASKS_LV];
};

/* ������������Ľṹ�� */
/* 
   now_lv	��ǰ�����������Ĳ��ϣ�lv==level��
   lv_change	�´������л�ʱ,�Ƿ���Ҫ�޸�level
   level[]	���е�level�������ڸ�������
   tasks0	ע�⣡ϵͳ�е�������Ȼ������ͬ�Ĳ㣬�����κα�ʶһ������Ľṹ�嶼������������е�
 */
struct TASKCTL 
{
	int now_lv; 
	char lv_change; 
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

/*----����������Ϣ------------------------------------------*/
struct WINDOW_INFO
{
	int nXSize,nYSize;
	char title[20];
	struct TASK task;
};
/*----����λͼ���------------------------------------------*/
typedef unsigned char BYTE;  
typedef unsigned short WORD;  
typedef unsigned int DWORD;  
typedef int LONG;  
  
  
//λͼ�ļ�ͷ��Ϣ�ṹ����  
//���в������ļ�������Ϣ�����ڽṹ����ڴ�ṹ������Ҫ�Ǽ��˵Ļ���������ȷ��ȡ�ļ���Ϣ��  
  
typedef struct tagBITMAPFILEHEADER 
{  
//WORD bfType;//�̶�Ϊ0x4d42;
DWORD bfSize;           //�ļ���С  
WORD   bfReserved1;     //�����֣�������  
WORD   bfReserved2;     //�����֣�ͬ��  
DWORD bfOffBits;        //ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��  
} BITMAPFILEHEADER;   
  
  
//��ϢͷBITMAPINFOHEADER��Ҳ��һ���ṹ���䶨�����£�  
  
typedef struct tagBITMAPINFOHEADER
{  
//public:  
DWORD   biSize;             //ָ���˽ṹ��ĳ��ȣ�Ϊ40  
LONG    biWidth;            //λͼ��  
LONG    biHeight;           //λͼ��  
WORD    biPlanes;           //ƽ������Ϊ1  
WORD    biBitCount;         //������ɫλ����������1��2��4��8��16��24���µĿ�����32  
DWORD   biCompression;      //ѹ����ʽ��������0��1��2������0��ʾ��ѹ��  
DWORD   biSizeImage;        //ʵ��λͼ����ռ�õ��ֽ���  
LONG    biXPelsPerMeter;    //X����ֱ���  
LONG    biYPelsPerMeter;    //Y����ֱ���  
DWORD   biClrUsed;          //ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)  
DWORD   biClrImportant;     //��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��  
} BITMAPINFOHEADER;   
  
  
//��ɫ��Palette����Ȼ�������Ƕ���Щ��Ҫ��ɫ���λͼ�ļ����Եġ�24λ��32λ�ǲ���Ҫ��ɫ��ġ�  
//���ƺ��ǵ�ɫ��ṹ���������ʹ�õ���ɫ������  
  
typedef struct tagRGBQUAD 
{   
//public:  
BYTE     rgbBlue; //����ɫ����ɫ����  
BYTE     rgbGreen; //����ɫ����ɫ����  
BYTE     rgbRed; //����ɫ�ĺ�ɫ����  
BYTE     rgbReserved; //����ֵ  
} RGBQUAD;  
  

#endif
