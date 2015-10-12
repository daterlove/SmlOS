#ifndef _TIMER_H
#define _TIMER_H


#define TIMER_FLAGS_ALLOC		1	/* 已配置状态*/
#define TIMER_FLAGS_USING		2	/* 定时器运行中 */

/* 初始化pit函数 */
void init_pit(void);
/* 创建新的定时器并返回指向该定时器结构的指针 */
struct TIMER *timer_alloc(void);

/* 释放timer所指向的定时器 */
void timer_free(struct TIMER *timer);
/* 定时器结构的初始化 */
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned char data);
/* 定时器的设定 */
void timer_settime(struct TIMER *timer, unsigned int timeout);


#endif
