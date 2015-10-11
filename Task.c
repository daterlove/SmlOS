
#include "common.h"

struct TASKCTL *taskctl;
//struct TIMER *mt_timer;

struct TIMER *task_timer;
//int mt_tr;							/* 存放TSS的选择子 */

/* 初始化函数 */
struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));

	/* 初始化所有的任务的task结构 */
	for (i = 0; i < MAX_TASKS; i++) 
	{
		/*正在使用，未运行标志-0*/
		taskctl->tasks0[i].flags = 0;
		/* 选择子初始化 */
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		/* 描述符初始化 */
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}
	
	task = task_alloc();
	task->flags = 2; /* 活动中标志 */
	task->priority = 2; /* 0.02秒 */
	taskctl->running = 1;/*运行个数为1*/
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	
	load_tr(task->sel);/* 修改tr寄存器 */
	
	task_timer = timer_alloc();/*任务切换的定时器 */
	
	timer_settime(task_timer, task->priority);/*设定任务优先级时间*/
	return task;
}

/*任务分配函数*/
struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	
	/* 遍历所有的task结构 */
	for (i = 0; i < MAX_TASKS; i++) 
	{
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 休眠中 */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			
			task->tss.eax = 0; /* 初始化各个寄存器 */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; /*全部在使用中*/
}

/*设置任务运行状态*/
void task_run(struct TASK *task, int priority)
{
	if (priority > 0) 
	{
		task->priority = priority;/*设定运行优先级*/
	}
	if (task->flags != 2) /*任务没有在运行*/
	{
		task->flags = 2; /*设置运行状态*/
		taskctl->tasks[taskctl->running] = task;
		taskctl->running++;
	}
	return;
}

/*任务切换函数*/
void task_switch(void)
{
	struct TASK *task;
	taskctl->now++;
	if (taskctl->now == taskctl->running)
	{
		/*如果任务循环到最后一个，切换到第一个*/
		taskctl->now = 0;
	}
	
	task = taskctl->tasks[taskctl->now];
	timer_settime(task_timer, task->priority);/*按优先级，设置时间片时间*/
	
	if (taskctl->running >= 2) /*任务数大于2时切换*/
	{
		farjmp(0, task->sel);
	}
	return;
}

/*任务休眠函数*/
void task_sleep(struct TASK *task)
{
	int i;
	char ts = 0;
	if (task->flags == 2)/* 如果处于活动状态 */
	{		
		/*如果休眠本身任务，稍后进行任务切换*/
		if (task == taskctl->tasks[taskctl->now])
		{
			ts = 1; 
		}
		
		/*寻找task所在位置*/
		for (i = 0; i < taskctl->running; i++) 
		{
			/*找到就结束*/
			if (taskctl->tasks[i] == task) 
			{
				break;
			}
		}
		taskctl->running--;/*运行个数减1*/
		if (i < taskctl->now) 
		{
			taskctl->now--;/*需要移动成员，相应处理*/
		}
		
		/*移动成员*/
		for (; i < taskctl->running; i++) 
		{
			taskctl->tasks[i] = taskctl->tasks[i + 1];
		}
		task->flags = 1; /*设置未工作状态*/
		
		if (ts != 0)/*正在运行的任务执行切换*/
		{	
			if (taskctl->now >= taskctl->running)/*防止now出现错误，进行修正*/
			{
				taskctl->now = 0;
			}
			/*任务立即切换*/
			farjmp(0, taskctl->tasks[taskctl->now]->sel);
		}
	}
	return;
}