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
   col_inv			ͼ���͸��ɫɫ�� (��ν��ͼ���͸��ɫ:�����ͼ��Ϊ��,��HariMain�����зֱ������������������
									  sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);		�������ͼ����Ϣ
									  init_mouse_cursor8(buf_mouse, 99);		 			��ʼ�����ͼ�� 
									  sheet_setbuf�����һ������99�������������ͼ���col_inv��Ա���� ��͸��ɫ
									  ��init_mouse_cursor8�еĲ���99���������ͼ��������ʾ�����Ĳ��ֵ�ɫ��,����
									  ��ο��ú����Ķ���,��������ͼ�εľ����в��������״�Ĳ��ּ�ʹ�ø�ɫ��
									  Ҳ����˵����ɫ��Ϊ99�Ĳ��ֶ�����Ҫ�������)
   height			ͼ��ĸ߶�
   flags			��¼ͼ�����״̬��Ϣ
 */
struct SHEET 
{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
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
	unsigned char *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
#endif
