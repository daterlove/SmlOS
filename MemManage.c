#include "common.h"
/* �ڴ��⺯�� */
/* ����start��ַ��end��ַ��Χ��, �ܹ�ʹ�õ��ڴ��ĩβ��ַ */
unsigned int memtest(unsigned int start, unsigned int end)
{
	/*����Ĭ����486����CPU*/
	unsigned int cr0, i;

	/*��ֹ���ٻ���*/
	cr0 = load_cr0();			/* ��ȡCR0�Ĵ�����ֵ */
	cr0 |= CR0_CACHE_DISABLE; 	/* ����NW��CDλΪ1 */
	store_cr0(cr0);				/* д��CR0�Ĵ��� */
	
	i = memtest_sub(start, end);	/* ������naskfunc.nas�� */

	/*�������ٻ���*/
	cr0 = load_cr0();			/* ��ȡCR0�Ĵ�����ֵ */
	cr0 &= ~CR0_CACHE_DISABLE;  /* NW��CDλ���� */
	store_cr0(cr0);				/* д��CR0�Ĵ��� */
	
	return i;
}

/* �ڴ����ṹ��ʼ������ */
void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* �ڴ������Ϣ��Ŀ����Ŀ */
	man->maxfrees = 0;		/* frees�����ֵ */
	man->lostsize = 0;		/* �ͷ�ʧ�ܵ��ڴ��С�ܺ� */
	man->losts = 0;			/* �ͷ�ʧ�ܵĴ��� */
	return;
}

/* ���ؿ����ڴ��С���ܺ� */
unsigned int memman_total(struct MEMMAN *man)
{
	unsigned int i, nTotal = 0;
	for (i = 0; i < man->frees; i++) 
	{
		nTotal += man->free[i].size;
	}
	return nTotal;
}

/* �ڴ���亯�� */
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, addr;
	for (i = 0; i < man->frees; i++) 
	{
		/* �ҵ���һ���㹻����ڴ� */
		if (man->free[i].size >= size) 
		{
			
			addr = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			
			/* ���free[i]���0 �ͼ���һ��������Ϣ */
			if (man->free[i].size == 0) 
			{		
				man->frees--;
				for (; i < man->frees; i++) 
				{
					man->free[i] = man->free[i + 1]; /* ����ṹ�� */
				}
			}
			return addr;
		}
	}
	return 0; /* û�ÿ��ÿռ� */
}

/* �ڴ��ͷź��� */
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	/* Ϊ���ڹ����ڴ�  ��free[]����addr��˳������ */
	/* ���� �Ȳ���Ӧ�÷������� */
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
		/* ǰ���п����ڴ� */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) 
		{
			/* ������ǰ��Ŀ����ڴ���ɵ�һ�� �������� */
			man->free[i - 1].size += size;
			if (i < man->frees) 
			{
				/* ����Ҳ�� */
				if (addr + size == man->free[i].addr) 
				{
					/* Ҳ���������Ŀ����ڴ���ɵ�һ��  �������� */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]ɾ�� */
					/* free[i]���0����ɵ�ǰ��ȥ */
					man->frees--;
					for (; i < man->frees; i++) 
					{
						man->free[i] = man->free[i + 1]; /* �ṹ�帳ֵ */
					}
				}
			}
			return 0; 
		}
	}
	/* ������ǰ��Ŀ����ڴ���ɵ�һ�� */
	if (i < man->frees) 
	{
		/* ���滹�� */
		if (addr + size == man->free[i].addr)
		{
			/* �������������ݹ��ɵ�һ�� */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; 
		}
	}
	/* �Ȳ�����ǰ����ɵ�һ�� Ҳ�����������ɵ�һ�� �� û������Ҳû������*/
	if (man->frees < MEMMAN_FREES) 
	{
		/* free[i]֮��� ����ƶ� �ڳ�һ����ÿռ� */
		for (j = man->frees; j > i; j--) 
		{
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) 
		{
			man->maxfrees = man->frees; /* �������ֵ */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;
	}
	/* ���������ƶ� */
	man->losts++;
	man->lostsize += size;
	return -1; /* ʧ�� */
}

/* �ڴ���亯��  4KΪ��λ */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;		/* ������4Kȡ�� */
	a = memman_alloc(man, size);
	return a;
}

/* �ڴ��ͷź���	4KΪ��λ */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;		/* ������4Kȡ�� */
	i = memman_free(man, addr, size);
	return i;
}


