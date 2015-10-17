#include "common.h"
/* 内存检测函数 */
/* 检测从start地址到end地址范围内, 能够使用的内存的末尾地址 */
unsigned int memtest(unsigned int start, unsigned int end)
{
	/*这里默认是486以上CPU*/
	unsigned int cr0, i;

	/*禁止高速缓存*/
	cr0 = load_cr0();			/* 获取CR0寄存器的值 */
	cr0 |= CR0_CACHE_DISABLE; 	/* 设置NW与CD位为1 */
	store_cr0(cr0);				/* 写回CR0寄存器 */
	
	i = memtest_sub(start, end);	/* 定义在naskfunc.nas中 */

	/*开启高速缓存*/
	cr0 = load_cr0();			/* 获取CR0寄存器的值 */
	cr0 &= ~CR0_CACHE_DISABLE;  /* NW与CD位清零 */
	store_cr0(cr0);				/* 写回CR0寄存器 */
	
	return i;
}

/* 内存管理结构初始化函数 */
void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* 内存可用信息条目的数目 */
	man->maxfrees = 0;		/* frees的最大值 */
	man->lostsize = 0;		/* 释放失败的内存大小总和 */
	man->losts = 0;			/* 释放失败的次数 */
	return;
}

/* 返回空闲内存大小的总和 */
unsigned int memman_total(struct MEMMAN *man)
{
	unsigned int i, nTotal = 0;
	for (i = 0; i < man->frees; i++) 
	{
		nTotal += man->free[i].size;
	}
	return nTotal;
}

/* 内存分配函数 */
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, addr;
	for (i = 0; i < man->frees; i++) 
	{
		/* 找到第一个足够大的内存 */
		if (man->free[i].size >= size) 
		{
			
			addr = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			
			/* 如果free[i]变成0 就减掉一条可用信息 */
			if (man->free[i].size == 0) 
			{		
				man->frees--;
				for (; i < man->frees; i++) 
				{
					man->free[i] = man->free[i + 1]; /* 代入结构体 */
				}
			}
			return addr;
		}
	}
	return 0; /* 没用可用空间 */
}

/* 内存释放函数 */
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	/* 为便于归纳内存  将free[]按照addr的顺序排列 */
	/* 所以 先查找应该放在哪里 */
	for (i = 0; i < man->frees; i++) 
	{
		if (man->free[i].addr > addr) 
		{
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) 
	{
		/* 前面有可用内存 */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) 
		{
			/* 可以与前面的可用内存归纳到一起 即有上邻 */
			man->free[i - 1].size += size;
			if (i < man->frees) 
			{
				/* 后面也有 */
				if (addr + size == man->free[i].addr) 
				{
					/* 也可以与后面的可用内存归纳到一起  即有下邻 */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]删除 */
					/* free[i]变成0后归纳到前面去 */
					man->frees--;
					for (; i < man->frees; i++) 
					{
						man->free[i] = man->free[i + 1]; /* 结构体赋值 */
					}
				}
			}
			return 0; 
		}
	}
	/* 不能与前面的可用内存归纳到一起 */
	if (i < man->frees) 
	{
		/* 后面还有 */
		if (addr + size == man->free[i].addr)
		{
			/* 可以与后面的内容归纳到一起 */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; 
		}
	}
	/* 既不能与前面归纳到一起 也不能与后面归纳到一起 即 没有上邻也没有下邻*/
	if (man->frees < MEMMAN_FREES) 
	{
		/* free[i]之后的 向后移动 腾出一点可用空间 */
		for (j = man->frees; j > i; j--) 
		{
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) 
		{
			man->maxfrees = man->frees; /* 更新最大值 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;
	}
	/* 不能往后移动 */
	man->losts++;
	man->lostsize += size;
	return -1; /* 失败 */
}

/* 内存分配函数  4K为单位 */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;		/* 向上以4K取整 */
	a = memman_alloc(man, size);
	return a;
}

/* 内存释放函数	4K为单位 */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;		/* 向上以4K取整 */
	i = memman_free(man, addr, size);
	return i;
}


