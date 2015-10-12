/*该文件定义 先进先出 缓冲区队列 相关的操作*/

#include "common.h"


/*-----fifo32缓冲区---------------------------------------------------------------*/
/* 缓冲区初始化函数 */
/*
	初始化缓冲区结构体fifo, size表示缓冲区大小, buf表示缓冲区地址
 */
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 缓冲区的大小 */
	fifo->flags = 0;
	fifo->p = 0; /* 下一个数据写入位置 */
	fifo->q = 0; /* 下一个数据读出位置 */
	return;
}

/* 缓冲区写入函数 */
/*
	向缓冲区结构体fifo中的缓冲区写入数据data
 */
int fifo8_put(struct FIFO8 *fifo, unsigned char data)
{
	if (fifo->free == 0) 
	{				/* 如果缓冲区已满 */
		fifo->flags |= FLAGS_OVERRUN;	/* 设置溢出标志 */
		return -1;				
	}
	fifo->buf[fifo->p] = data;			/* 数据写入缓冲区 */
	fifo->p++;							/* 调整下一个写入位置 */
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;						/* 缓冲区空闲字节数减1 */
	return 0;
}


/* 	缓冲区读出函数 ,
	读取缓冲区结构体fifo中的缓冲区的一个字节数据 
	并作为函数返回值返回给调用者
 */
int fifo8_get(struct FIFO8 *fifo)
{
	int data;
	if (fifo->free == fifo->size) {		/* 缓冲区为空 没有数据可读 */
		return -1;
	}
	data = fifo->buf[fifo->q];			/* 读出数据 */
	fifo->q++;							/* 调整下一个读出位置 */
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;						/* 缓冲区空闲字节数加1 */
	return data;						/* 返回刚刚读出的数据 */
}

/* 返回缓冲区结构体fifo中的缓冲区中共有多少字节的数据 */
int fifo8_status(struct FIFO8 *fifo)
{
	return fifo->size - fifo->free;
}

/*-----fifo32缓冲区（比fifo8缓冲区单个数据换成int，且增加task成员）------*/
/* 缓冲区初始化函数 */
/*
	初始化缓冲区结构体fifo, size表示缓冲区大小, buf表示缓冲区地址
	当缓冲区有数据写入时需要唤醒task标识的任务
 */
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; 
	fifo->flags = 0;
	fifo->p = 0; /* 下一个数据写入位置 */
	fifo->q = 0; /* 下一个数据读出位置 */
	fifo->task = task; 
	return;
}

/* 缓冲区写入函数 */
/*
	向缓冲区结构体fifo中的缓冲区写入数据data
 */
int fifo32_put(struct FIFO32 *fifo, int data)
{
	if (fifo->free == 0)/* 如果缓冲区已满 */
	{				
		fifo->flags |= FLAGS_OVERRUN;	/* 设置溢出标志 */
		return -1;
	}
	fifo->buf[fifo->p] = data;			/* 数据写入缓冲区 */
	fifo->p++;							/* 调整下一个写入位置 */
	if (fifo->p == fifo->size) 
	{
		fifo->p = 0;
	}
	fifo->free--;						/* 缓冲区空闲数减一 */
	if (fifo->task != 0) /* 如果有需要唤醒的任务 */
	{				
		if (fifo->task->flags != 2)/* 如果任务处于休眠 */ 
		{ 
			task_run(fifo->task,fifo->task->priority); /* 唤醒该任务--这里需要测试 */
		}
	}
	return 0;
}

/* 缓冲区读出函数 */
/*
	读取缓冲区结构体fifo中的缓冲区的一个字节数据 
	并作为函数返回值返回给调用者
 */
int fifo32_get(struct FIFO32 *fifo)
{
	int data;
	if (fifo->free == fifo->size) /* 缓冲区为空 没有数据可读 */
	{
		
		return -1;
	}
	data = fifo->buf[fifo->q];			/* 读出数据 */
	fifo->q++;							/* 调整下一个读出位置 */
	if (fifo->q == fifo->size)
	{
		fifo->q = 0;
	}
	fifo->free++;						/* 缓冲区空闲数加1 */
	return data;						/* 返回刚刚读出的数据 */
}

/* 返回缓冲区结构体fifo中的缓冲区中共有多少数据 */
int fifo32_status(struct FIFO32 *fifo)
{
	return fifo->size - fifo->free;
}
