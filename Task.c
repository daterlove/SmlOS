
#include "common.h"

struct TIMER *mt_timer;
int mt_tr;							/* 存放TSS的选择子 */

/* 初始化函数 */
void mt_init(void)
{
	mt_timer = timer_alloc();		/* 创建一个定时器 */
	/* 这里没必要使用timer_init */
	timer_settime(mt_timer, 2);
	mt_tr = 3 * 8;					
	return;
}

/* 任务切换函数 */
void mt_taskswitch(void)
{
	if (mt_tr == 3 * 8) 
	{
		mt_tr = 4 * 8;
	} else 
	{
		mt_tr = 3 * 8;
	}
	timer_settime(mt_timer, 2);
	farjmp(0, mt_tr);				/* 远跳转 */
	return;
}