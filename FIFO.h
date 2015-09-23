#ifndef _FIFO_H
#define _FIFO_H

#define FLAGS_OVERRUN		0x0001			/* �����������־ */

/* FIFO�������ṹ�� */
struct FIFO8
{						
	unsigned char *buf;	/* ������ָ�� */
	int p;				/* p ��һ������д��λ��*/
	int q; 				/*q ��һ�����ݶ���λ��*/
	int size; 			/*size �����������ֽ���	*/
	int free; 			/*free �������Ŀ����ֽ���*/
	int flags;			/*flags	��¼�������Ƿ����*/
};

/* ��������ʼ������ 
��ʼ���������ṹ��fifo, size��ʾ��������С, buf��ʾ��������ַ
 */
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);

/* ������д�뺯�� 
�򻺳����ṹ��fifo�еĻ�����д������data
 */
int fifo8_put(struct FIFO8 *fifo, unsigned char data);

/* 	�������������� ,
��ȡ�������ṹ��fifo�еĻ�������һ���ֽ����� 
����Ϊ��������ֵ���ظ�������
 */
int fifo8_get(struct FIFO8 *fifo);

/* ���ػ������ṹ��fifo�еĻ������й��ж����ֽڵ����� */
int fifo8_status(struct FIFO8 *fifo);

#endif
