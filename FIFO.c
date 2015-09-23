/*���ļ����� �Ƚ��ȳ� ���������� ��صĲ���*/

#include "common.h"



/* ��������ʼ������ */
/*
	��ʼ���������ṹ��fifo, size��ʾ��������С, buf��ʾ��������ַ
 */
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* �������Ĵ�С */
	fifo->flags = 0;
	fifo->p = 0; /* ��һ������д��λ�� */
	fifo->q = 0; /* ��һ�����ݶ���λ�� */
	return;
}

/* ������д�뺯�� */
/*
	�򻺳����ṹ��fifo�еĻ�����д������data
 */
int fifo8_put(struct FIFO8 *fifo, unsigned char data)
{
	if (fifo->free == 0) 
	{				/* ������������� */
		fifo->flags |= FLAGS_OVERRUN;	/* ���������־ */
		return -1;				
	}
	fifo->buf[fifo->p] = data;			/* ����д�뻺���� */
	fifo->p++;							/* ������һ��д��λ�� */
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;						/* �����������ֽ�����1 */
	return 0;
}


/* 	�������������� ,
	��ȡ�������ṹ��fifo�еĻ�������һ���ֽ����� 
	����Ϊ��������ֵ���ظ�������
 */
int fifo8_get(struct FIFO8 *fifo)
{
	int data;
	if (fifo->free == fifo->size) {		/* ������Ϊ�� û�����ݿɶ� */
		return -1;
	}
	data = fifo->buf[fifo->q];			/* �������� */
	fifo->q++;							/* ������һ������λ�� */
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;						/* �����������ֽ�����1 */
	return data;						/* ���ظոն��������� */
}

/* ���ػ������ṹ��fifo�еĻ������й��ж����ֽڵ����� */
int fifo8_status(struct FIFO8 *fifo)
{
	return fifo->size - fifo->free;
}
