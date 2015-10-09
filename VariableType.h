#ifndef _VARIABLETYPE_H
#define _VARIABLETYPE_H

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

/*---------------------------------------------------------------*/
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

/*---------------------------------------------------------------*/
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

/*---------------------------------------------------------------*/
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
	struct FIFO8 *fifo;
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

/*---------------------------------------------------------------*/
/* TSS�ṹ ��Task-State Segment)*/
struct TSS32 
{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

#endif
