#ifndef _TASK_H_
#define _TASK_H_

/* 初始化函数 */
struct TASK *task_init(struct MEMMAN *memman);
/*任务分配函数*/
struct TASK *task_alloc(void);
/*设置任务运行状态*/
void task_run(struct TASK *task, int priority);
/*任务切换函数*/
void task_switch(void);
/*任务休眠函数*/
void task_sleep(struct TASK *task);

#endif
