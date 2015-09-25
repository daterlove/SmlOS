#include "common.h"
/* 标识图层正在使用符号常量 */
#define SHEET_USE		1

/* 图层管理结构初始化函数 */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));	/* 分配内存 */
	if (ctl == 0) 
	{					/* 如果内存分配失败 */
		goto err;
	} 
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* 一个图层都没有 */
	for (i = 0; i < MAX_SHEETS; i++) 
	{
		ctl->sheets0[i].flags = 0; /* 标记为未使用 */
	} 
err:
	return ctl;
}

/* 创建新的图层 */
struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) 
	{
		if (ctl->sheets0[i].flags == 0) 
		{	/* 查找sheets0数组中第一个未使用的图层 */
			sht = &ctl->sheets0[i];			
			sht->flags = SHEET_USE; /* 标识为正在使用 */
			sht->height = -1; /* 隐藏 高度为-1 其实就是表示该图层不在屏幕上显示 */
			return sht;
		}
	}
	return 0;	
}

/* 图层设置函数 */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

/* 重画所有图层的某一矩形区域 */
/* vx0, vy1定义该矩形区域的左上角坐标 */
/* vx1, vy1定义该矩形区域的右下角坐标 */
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1)
{
	/* 以b开头的变量定义了图形在缓冲区中的坐标 */
	/* 以v开头的变量定义了图形在屏幕上的坐标 */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram = ctl->vram;
	struct SHEET *sht;
	for (h = 0; h <= ctl->top; h++) {		/* 遍历所有图层 */
		sht = ctl->sheets[h];
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;			/* 计算图形在缓冲区中的坐标 */
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		/* 关于以下四条if语句的用意真的很难用言语来表达... */
		
		if (bx0 < 0) { bx0 = 0; }		/* 左上角坐标调整 */
		if (by0 < 0) { by0 = 0; }	
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }	/* 右下角坐标调整 */
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		
		for (by = by0; by < by1; by++) 
		{
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) 
			{
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx];
				if (c != sht->col_inv) 
				{
					vram[vy * ctl->xsize + vx] = c;	
				}
			}
		}
	}
	return;
}

/* 图层高度设定函数 */
/* 事实上,这就是一个有序结构体数组的插入与删除的算法的实现 */
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height; /* 保存原来的高度 */

	/* 如果新的高度比所有图层的最大的高度还大 则调整它 */
	if (height > ctl->top + 1) 
	{
		height = ctl->top + 1;
	}
	/* 如果新高度小于-1 就设置为-1 我们只需要用-1来表示隐藏就足够了 */
	if (height < -1) 
	{
		height = -1;
	}
	sht->height = height; /* 更新图层的高度 */

	/* 对sheets[]的排列(sheets[]就是一个按高度排序的有序数组) */
	if (old > height) /* 原来的高度比新高度还要大 */
	{	
		if (height >= 0) 
		{	/* 不是要隐藏原来图层 */
			/* 把中间的往上提 */
			for (h = old; h > height; h--)
			{ /* 我们可以认为高度就是图层在sheets数组中的下标 */
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;		/* 调整原来sheets[]中指向的图层的高度 */
			}									/* 即更改原来的图层在sheets[]中的下标 */
			ctl->sheets[height] = sht;			/* 插入新的图层指针 */
		} 
		else 
		{	/* 隐藏原来图层 */
			if (ctl->top > old) 
			{		/* 如果原来的图层不是最上层的图层 */
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++) 
				{
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;	/* 调整原来sheets[]中指向的图层的高度 */
				}
			}
			ctl->top--; /* 由于显示中的图层减少了一个, 所以最上层的图层高度减一 */
		}
		/* 描绘这个图层 */
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	else if (old < height) 
	{	/* 原来的高度比新高度还要小 */
		if (old >= 0)
		{			/* 原来的图层如果不是隐藏的 */
			/* 把中间的拉下去 */
			for (h = old; h < height; h++) 
			{
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;		/* 调整原来sheets[]中指向的图层的高度 */
			}
			ctl->sheets[height] = sht;			/* 插入新的图层指针 */
		} 
		else 
		{	/* 原来的图层是隐藏的 */
			for (h = ctl->top; h >= height; h--) 
			{
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;	/* 调整原来sheets[]中指向的图层的高度 */
			}
			ctl->sheets[height] = sht;			/* 插入新的图层指针 */
			ctl->top++; /* 由于增加了一个要显示的图层,所以最上层的图层高度需要加一 */
		}
		/* 描绘这个图层 */
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize); 
	}
	return;
}


/* 刷新某图层缓冲区中的一个矩形区域 */
/* 注意！此处给出的4个坐标参数都是缓冲区中的坐标 */
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) /* 如果图层不是隐藏的 */
	{ 
		/* 计算出实际坐标并调用sheet_refreshsub对该区域进行重画 */
		sheet_refreshsub(ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
	return;
}

/* 移动图层的函数 */
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	
	sht->vx0 = vx0;			/* 设置图层新的坐标 */
	sht->vy0 = vy0;
	
	if (sht->height >= 0) /* 如果该图层不是一个隐藏图层 */
	{ 
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
	return;
}

/* 释放某一图层的函数 */
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht)
{
	if (sht->height >= 0) /* 如果这个图层是显示的 */
	{			
		sheet_updown(ctl, sht, -1); /* 修改其高度为-1 即隐藏它 */
	}
	sht->flags = 0; /* 设置为未使用状态 */
	return;
}

