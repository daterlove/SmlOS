#include "common.h"

/* 标识图层正在使用符号常量 */
#define SHEET_USE		1

/* 图层管理结构初始化函数 */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
	/* 为图层管理结构分配内存 */
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));
	if (ctl == 0) /* 如果分配失败 */
	{	
		goto err;
	}
	/* 为map分配内存,注意分配的大小是xsize*ysize对4K向上取整,它可以描绘出整个屏幕的图层信息 */
	ctl->map = (unsigned char *) memman_alloc_4k(memman, xsize * ysize);
	if (ctl->map == 0) /* 如果分配失败 */
	{	
		/* 释放最先分配的SHTCTL结构体的内存 */
		memman_free_4k(memman, (int) ctl, sizeof (struct SHTCTL));
		goto err;
	}
	ctl->vram = vram;		
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* 一个图层也没有 */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets[i].flags = 0; /* 标记为未使用 */
		ctl->sheets[i].ctl = ctl; /* 记录所属 */
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
		/* 遍历整个sheets数组查找第一个未使用的图层 */
		if (ctl->sheets[i].flags == 0)
		{	
			sht = &ctl->sheets[i];
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

/* 设置屏幕上某一区域的map信息 */
/* 从高度h0~top的图层中选择某些图层,将其sid值填土该map区域 */
/* vx0,vy0定义了该区域的左上角坐标	vx1,vy1定义了该区域的右下角坐标 */
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
	/* 以b开头的变量定义了图形在缓冲区中的坐标 */
	/* 以v开头的变量定义了图形在屏幕上的坐标 */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, sid, *map = ctl->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	
	for (h = h0; h <= ctl->top; h++) 
	{		/* 遍历从h0到top的所有图层 */
		sht = ctl->p_sheets[h];
		sid = sht - ctl->sheets; /* sid是图层在sheets中的下标 */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;		/* 计算图层在缓冲区中的坐标 */
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		
		if (bx0 < 0) { bx0 = 0; }	/* 左上角坐标调整 */
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }	/* 右下角坐标调整 */
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		
		for (by = by0; by < by1; by++) 
		{	
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) 
			{
				vx = sht->vx0 + bx;
				/* 如果这一个坐标上该图层是要显示的,我们才会将其sid写入map中 */
				/* 后面更高的图层可能会覆盖这个图层写入的sid值 */
				if (buf[by * sht->bxsize + bx] != sht->col_inv) 
				{
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
	}
	return;
}

/* 重画屏幕上的某一区域 */
/* 根据map中这一区域的值选择高度从h0到h1中的某些图层进行重画 */
/* vx0, vy1定义该矩形区域的左上角坐标 */
/* vx1, vy1定义该矩形区域的右下角坐标 */
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	/* 以b开头的变量定义了图形在缓冲区中的坐标 */
	/* 以v开头的变量定义了图形在屏幕上的坐标 */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET *sht;
	/* 如果refresh的范围超出了画面 则修正 */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	
	for (h = h0; h <= h1; h++) 
	{	/* 遍历图层 */
		sht = ctl->p_sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets;
		/* 计算图形在缓冲区中的坐标 */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }	/* 左上角坐标调整 */
		if (by0 < 0) { by0 = 0; }	
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }	/* 右下角坐标调整 */
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		
		for (by = by0; by < by1; by++) 
		{
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) 
			{
				vx = sht->vx0 + bx;
				/* 如果该坐标的map值与sid相等 才重画 */
				if (map[vy * ctl->xsize + vx] == sid) 
				{	
					vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		}
	}
	return;
}

/* 图层高度设定函数 */
/* 事实上,这就是一个有序结构体数组的插入与删除的算法的实现 */
/* 若对算法不明白的可参考任意一本关于数据结构的书籍 */
void sheet_updown(struct SHEET *sht, int height)
{
	struct SHTCTL *ctl = sht->ctl;
	int h, old = sht->height; /* 保存原来的高度 */

	/* 如果新的高度比所有图层的最大的高度还大 则调整它 */
	if (height > ctl->top + 1) 
	{
		height = ctl->top + 1;
	}
	/* 如果新高度小于-1 就设置为-1 我们只需要用-1来表示隐藏就足够了 */
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* 更新图层的高度 */

	/* 对p_sheets[]的排列(p_sheets[]就是一个按高度排序的有序数组) */
	if (old > height) 
	{	/* 原来的高度比新高度还要大 */
		if (height >= 0) 
		{
			/* 把中间的往上提 */
			/* 我们可以认为高度就是图层在p_sheets数组中的下标 */
			for (h = old; h > height; h--) 
			{	
				ctl->p_sheets[h] = ctl->p_sheets[h - 1];
				ctl->p_sheets[h]->height = h;		/* 调整原来p_sheets[]中指向的图层的高度 */
												/* 即更改原来的图层在p_sheets[]中的下标 */
			}
			ctl->p_sheets[height] = sht;			/* 插入新的图层指针 */
			/* 设置map信息并重画 */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} 
		else 
		{	/* 隐藏原来图层 */
			/* 如果原来的图层不是最上层的图层 */
			if (ctl->top > old) 
			{	
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++)
				{
					ctl->p_sheets[h] = ctl->p_sheets[h + 1];
					ctl->p_sheets[h]->height = h;	/* 调整原来p_sheets[]中指向的图层的高度 */
				}
			}
			ctl->top--; /* 由于显示中的图层减少了一个, 所以最上层的图层高度减一 */
			/* 设置map信息并重画 */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
		}
	} 
	else if (old < height) 
	{	/* 原来的高度比新高度还要小 */
		if (old >= 0) 
		{			/* 原来的图层如果不是隐藏的 */
			/* 把中间的拉下去 */
			for (h = old; h < height; h++) 
			{
				ctl->p_sheets[h] = ctl->p_sheets[h + 1];
				ctl->p_sheets[h]->height = h;		/* 调整原来p_sheets[]中指向的图层的高度 */
			}
			ctl->p_sheets[height] = sht;
			
		} 
		else 
		{	/* 原来的图层是隐藏的 */
			for (h = ctl->top; h >= height; h--) 
			{
				ctl->p_sheets[h + 1] = ctl->p_sheets[h];
				ctl->p_sheets[h + 1]->height = h + 1;		/* 调整原来p_sheets[]中指向的图层的高度 */
			}
			ctl->p_sheets[height] = sht;		/* 插入新的图层指针 */
			ctl->top++; /* 由于增加了一个要显示的图层,所以最上层的图层高度需要加一 */
		}
		/* 设置map信息并重画 */
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
	}
	return;
}

/* 刷新某图层缓冲区中的一个矩形区域 */
/* 注意！此处给出的4个坐标参数都是缓冲区中的坐标 */
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	/* 如果图层不是隐藏的 */
	if (sht->height >= 0) 
	{ 
		/* 计算出实际坐标并调用sheet_refreshsub对该区域进行重画 */
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
	return;
}

/* 移动图层的函数 */
/* 
	sht是需要被移动的图层
	vx0,vy1是被移动图层新的左上角坐标
 */
void sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
	struct SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;	/* 保存图层原来的坐标 */
	sht->vx0 = vx0;			/* 设置图层新的坐标 */
	sht->vy0 = vy0;
	
	/* 如果该图层不是一个隐藏图层 */
	if (sht->height >= 0) 
	{ 
		/* 先修改map信息 再进行重画 */
		/* 该语句相当于将map中(old_vx0, old_vy0)(old_vx0 + sht->bxsize, old_vy0 + sht->bysize)定义的
		   矩形区域重新设置,注意:此时sht图层的坐标已经修改了,而且此处给出的高度为0.也就是说从高度为0
		   到top的所有图层都会判断是否需要将自己的sid值写入map中该区域
		   */
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		/* 与上一条语句类似,此时给出的高度是图层本身的高度 至于为什么请参考书中P213中间的一段话 */
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		/* 修改了map之后根据map中的值来重画 */
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
	return;
}

/* 释放某一图层的函数 */
void sheet_free(struct SHEET *sht)
{
	if (sht->height >= 0)
	{		/* 如果这个图层是显示的 */
		sheet_updown(sht, -1);  /* 修改其高度为-1 即隐藏它 */
	}
	sht->flags = 0; /* 设置为未使用状态 */
	return;

}

