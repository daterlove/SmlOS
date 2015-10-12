#ifndef _FIFO_H
#define _FIFO_H

#define FLAGS_OVERRUN		0x0001			/* 缓冲区溢出标志 */



/* 缓冲区初始化函数 
初始化缓冲区结构体fifo, size表示缓冲区大小, buf表示缓冲区地址
 */
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);

/* 缓冲区写入函数 
向缓冲区结构体fifo中的缓冲区写入数据data
 */
int fifo8_put(struct FIFO8 *fifo, unsigned char data);

/* 	缓冲区读出函数 ,
读取缓冲区结构体fifo中的缓冲区的一个字节数据 
并作为函数返回值返回给调用者
 */
int fifo8_get(struct FIFO8 *fifo);

/* 返回缓冲区结构体fifo中的缓冲区中共有多少字节的数据 */
int fifo8_status(struct FIFO8 *fifo);

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

#endif
