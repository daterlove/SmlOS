#ifndef _FIFO_H
#define _FIFO_H

#define FLAGS_OVERRUN		0x0001			/* 缓冲区溢出标志 */

/* FIFO缓冲区结构体 */
struct FIFO8
{						
	unsigned char *buf;	/* 缓冲区指针 */
	int p;				/* p 下一个数据写入位置*/
	int q; 				/*q 下一个数据读出位置*/
	int size; 			/*size 缓冲区的总字节数	*/
	int free; 			/*free 缓冲区的空闲字节数*/
	int flags;			/*flags	记录缓冲区是否溢出*/
};

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

#endif
