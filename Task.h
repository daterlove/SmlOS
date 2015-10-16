#ifndef _TASK_H_
#define _TASK_H_

/* 获取当前正在运行的任务的task结构指针 */
struct TASK *task_now(void);
/* （根据level）添加任务 */
void task_add(struct TASK *task);
/* 从level删除任务（设flag=1)*/
void task_remove(struct TASK *task);
/* 设置最上层有任务的level */
void task_switchsub(void);
/* 分配一个task结构 */
struct TASK *task_alloc(void);
/* 初始化函数 */
struct TASK *task_init(struct MEMMAN *memman);
/*运行任务（level为负数时，使用原本level)*/
void task_run(struct TASK *task, int level, int priority);
/*任务休眠*/
void task_sleep(struct TASK *task);
/* 任务切换函数 */
void task_switch(void);

#endif
