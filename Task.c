
#include "common.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

/*flag 值：0：未分配，1：休眠，2：活动中*/

/* 闲置时任务 将被放在最底层 */
void main_syshlt(void)
{
	for (;;) 
	{
		io_hlt();
	}
}

/* 获取当前正在运行的任务的task结构指针 */
struct TASK *task_now(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];//当前任务层
	return tl->tasks[tl->now];//当前任务
}

/* （根据level）添加任务 */
void task_add(struct TASK *task)
{
	/*这里并未做超限检查，需要上层调用判断running个数*/
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;	
	tl->running++;
	task->flags = 2; /* 活动中标记  */
	return;
}

/* 从level删除任务（设flag=1)*/
void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	/* 寻址task所在位置 */
	for (i = 0; i < tl->running; i++) 
	{
		if (tl->tasks[i] == task) 
		{
			/* 找到就退出循环 */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) 
	{
		tl->now--; /* 它在now的前面,也就是说i后面的都要前移一个位置, 下标为now的结构当然也要前移 */
				   /* 所以先在这里就修改now的值, 指向移动后原来指向的结构的新位置 */
	}
	
	if (tl->now >= tl->running)/*如果now的值出现异常就修正*/ 
	{	
		tl->now = 0;
	}
	task->flags = 1; /* 休眠中 */

	/* 移位 */
	for (; i < tl->running; i++) 
	{
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}

/* 设置最上层有任务的level */
void task_switchsub(void)
{
	int i;
	/* 寻址最上层的level(运行任务不为0的)*/
	for (i = 0; i < MAX_TASKLEVELS; i++)
	{
		if (taskctl->level[i].running > 0) 
		{
			break; /* 找到就跳出循环 */
		}
	}
	taskctl->now_lv = i;/*设置当前level层*/
	taskctl->lv_change = 0;		
	return;
}

/* 分配一个task结构 */
struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	for (i = 0; i < MAX_TASKS; i++)/* 遍历所有的task结构 */ 
	{		
		if (taskctl->tasks0[i].flags == 0)/*未活动*/ 
		{	
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 休眠中 */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			
			/* 初始化各个寄存器 */
			task->tss.eax = 0; 	
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
	return 0; 
}

/* 初始化函数 */
struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task_syshlt;//闲置任务，使用临时变量，不用维护
	struct TASK *task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	/*申请任务管理结构体内存*/
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	
	/* 初始化任务结构体的task结构 */
	for (i = 0; i < MAX_TASKS; i++) 
	{
		taskctl->tasks0[i].flags = 0;		
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;		/* 选择子初始化 */
		
		/* 描述符初始化 */
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}
	
	/* 初始化所有level结构 */
	for (i = 0; i < MAX_TASKLEVELS; i++) 
	{
		taskctl->level[i].running = 0;	/* 没有正在运行的任务 */
		taskctl->level[i].now = 0;		
	}
	
	/*返回一个任务*/
	task = task_alloc();
	task->flags = 2;	/* 活动中标志 */
	task->priority = 2; /* 0.02秒 */
	task->level = 0;	/* 最高LEVEL */
	
	task_add(task);		//添加任务到level里	
	task_switchsub();	/* 设置当前level*/
	
	load_tr(task->sel);	/* 修改tr寄存器 */
	task_timer = timer_alloc();	/* 重头戏！！任务切换的定时器 */
	timer_settime(task_timer, task->priority);	
	
	/* 闲置任务的初始化(防止系统没有任务的时候系统奔溃)*/
	task_syshlt = task_alloc();
	task_syshlt->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;//任务堆栈
	task_syshlt->tss.eip = (int) &main_syshlt;
	task_syshlt->tss.es = 1 * 8;
	task_syshlt->tss.cs = 2 * 8;
	task_syshlt->tss.ss = 1 * 8;
	task_syshlt->tss.ds = 1 * 8;
	task_syshlt->tss.fs = 1 * 8;
	task_syshlt->tss.gs = 1 * 8;
	task_run(task_syshlt, MAX_TASKLEVELS - 1, 1);
	
	return task;
}

/*运行任务*/
/*level为负数时，使用原本level,priority=0,使用原来 priority*/
void task_run(struct TASK *task, int level, int priority)
{
	if (level < 0) 
	{
		level = task->level; /*不改变level */
	}
	if (priority > 0) 
	{		
		task->priority = priority;
	}

	/* 对于刚刚调用task_alloc()函数创建的任务，直接将其唤醒(因为默认刚创建的任务的flag = 1) */
	
	/* 对于已经可以运行(flag=2不代表正在运行,只能说它可能在运行也可能在队列中等待调度函数调度它来运行) */
	/* 的任务, 则还要判断它的level是否与参数给的level相等,相等则不需要修改。*/
	/* 若不相等, 则先将其从等待运行的队列中删除再修改其level */
	
	if (task->flags == 2 && task->level != level) 
	{ /* 改变活动中的任务的level */
		task_remove(task); /* 这里执行之后 flags的值会变为1，于是下面的if语句块也会被执行 */
	}
	if (task->flags != 2)
	{
		/* 从休眠状态唤醒的情形 */
		task->level = level;
		task_add(task);
	}
	/* 因为上面可能已经修改了某个任务的level 所以再下次调度时必须检测level */
	
	taskctl->lv_change = 1; /* 下次切换任务时检查level */
	return;
}

/*任务休眠*/
void task_sleep(struct TASK *task)
{
	struct TASK *now_task;
	if (task->flags == 2)/* 如果处于活动状态 */
	{		
		now_task = task_now();	/* 获得当前正在运行的任务的task指针 */
		task_remove(task); /* 执行词语句的话flag会等于1 */
		
		if (task == now_task)/* 如果是让自己休眠 则需要任务切换 */ 
		{			
			task_switchsub();
			now_task = task_now(); /* 在设定后获取新的level中待运行的任务 */
			farjmp(0, now_task->sel);	/* 切换任务 */
		}
	}
	return;
}

/* 任务切换函数 */
/* 该函数只在void inthandler20(int *esp)时钟中断处理中被调用 */
void task_switch(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];//当前层
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	
	tl->now++;
	if (tl->now == tl->running)/* 当now出现异常时调整它 */
	{
		tl->now = 0;
	}
	if (taskctl->lv_change != 0)/* 是否需要检测level */ 
	{	
		task_switchsub();	/* 寻找最上层的level */
		tl = &taskctl->level[taskctl->now_lv];	/* 更新tl让它指向最上层的level */
	}

	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	
	if (new_task != now_task)/*只有一个任务不跳转*/
	{
		farjmp(0, new_task->sel);
	}
	return;
}