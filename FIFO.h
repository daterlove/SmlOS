#ifndef _FIFO_H
#define _FIFO_H

#define FLAGS_OVERRUN		0x0001			/* �����������־ */



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

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

#endif
