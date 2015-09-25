#include "common.h"
/* ��ʶͼ������ʹ�÷��ų��� */
#define SHEET_USE		1

/* ͼ�����ṹ��ʼ������ */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));	/* �����ڴ� */
	if (ctl == 0) 
	{					/* ����ڴ����ʧ�� */
		goto err;
	} 
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* һ��ͼ�㶼û�� */
	for (i = 0; i < MAX_SHEETS; i++) 
	{
		ctl->sheets0[i].flags = 0; /* ���Ϊδʹ�� */
	} 
err:
	return ctl;
}

/* �����µ�ͼ�� */
struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) 
	{
		if (ctl->sheets0[i].flags == 0) 
		{	/* ����sheets0�����е�һ��δʹ�õ�ͼ�� */
			sht = &ctl->sheets0[i];			
			sht->flags = SHEET_USE; /* ��ʶΪ����ʹ�� */
			sht->height = -1; /* ���� �߶�Ϊ-1 ��ʵ���Ǳ�ʾ��ͼ�㲻����Ļ����ʾ */
			return sht;
		}
	}
	return 0;	
}

/* ͼ�����ú��� */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

/* �ػ�����ͼ���ĳһ�������� */
/* vx0, vy1����þ�����������Ͻ����� */
/* vx1, vy1����þ�����������½����� */
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1)
{
	/* ��b��ͷ�ı���������ͼ���ڻ������е����� */
	/* ��v��ͷ�ı���������ͼ������Ļ�ϵ����� */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram = ctl->vram;
	struct SHEET *sht;
	for (h = 0; h <= ctl->top; h++) {		/* ��������ͼ�� */
		sht = ctl->sheets[h];
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;			/* ����ͼ���ڻ������е����� */
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		/* ������������if����������ĺ��������������... */
		
		if (bx0 < 0) { bx0 = 0; }		/* ���Ͻ�������� */
		if (by0 < 0) { by0 = 0; }	
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }	/* ���½�������� */
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

/* ͼ��߶��趨���� */
/* ��ʵ��,�����һ������ṹ������Ĳ�����ɾ�����㷨��ʵ�� */
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height; /* ����ԭ���ĸ߶� */

	/* ����µĸ߶ȱ�����ͼ������ĸ߶Ȼ��� ������� */
	if (height > ctl->top + 1) 
	{
		height = ctl->top + 1;
	}
	/* ����¸߶�С��-1 ������Ϊ-1 ����ֻ��Ҫ��-1����ʾ���ؾ��㹻�� */
	if (height < -1) 
	{
		height = -1;
	}
	sht->height = height; /* ����ͼ��ĸ߶� */

	/* ��sheets[]������(sheets[]����һ�����߶��������������) */
	if (old > height) /* ԭ���ĸ߶ȱ��¸߶Ȼ�Ҫ�� */
	{	
		if (height >= 0) 
		{	/* ����Ҫ����ԭ��ͼ�� */
			/* ���м�������� */
			for (h = old; h > height; h--)
			{ /* ���ǿ�����Ϊ�߶Ⱦ���ͼ����sheets�����е��±� */
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;		/* ����ԭ��sheets[]��ָ���ͼ��ĸ߶� */
			}									/* ������ԭ����ͼ����sheets[]�е��±� */
			ctl->sheets[height] = sht;			/* �����µ�ͼ��ָ�� */
		} 
		else 
		{	/* ����ԭ��ͼ�� */
			if (ctl->top > old) 
			{		/* ���ԭ����ͼ�㲻�����ϲ��ͼ�� */
				/* ������Ľ����� */
				for (h = old; h < ctl->top; h++) 
				{
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;	/* ����ԭ��sheets[]��ָ���ͼ��ĸ߶� */
				}
			}
			ctl->top--; /* ������ʾ�е�ͼ�������һ��, �������ϲ��ͼ��߶ȼ�һ */
		}
		/* ������ͼ�� */
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	else if (old < height) 
	{	/* ԭ���ĸ߶ȱ��¸߶Ȼ�ҪС */
		if (old >= 0)
		{			/* ԭ����ͼ������������ص� */
			/* ���м������ȥ */
			for (h = old; h < height; h++) 
			{
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;		/* ����ԭ��sheets[]��ָ���ͼ��ĸ߶� */
			}
			ctl->sheets[height] = sht;			/* �����µ�ͼ��ָ�� */
		} 
		else 
		{	/* ԭ����ͼ�������ص� */
			for (h = ctl->top; h >= height; h--) 
			{
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;	/* ����ԭ��sheets[]��ָ���ͼ��ĸ߶� */
			}
			ctl->sheets[height] = sht;			/* �����µ�ͼ��ָ�� */
			ctl->top++; /* ����������һ��Ҫ��ʾ��ͼ��,�������ϲ��ͼ��߶���Ҫ��һ */
		}
		/* ������ͼ�� */
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize); 
	}
	return;
}


/* ˢ��ĳͼ�㻺�����е�һ���������� */
/* ע�⣡�˴�������4������������ǻ������е����� */
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) /* ���ͼ�㲻�����ص� */
	{ 
		/* �����ʵ�����겢����sheet_refreshsub�Ը���������ػ� */
		sheet_refreshsub(ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
	return;
}

/* �ƶ�ͼ��ĺ��� */
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	
	sht->vx0 = vx0;			/* ����ͼ���µ����� */
	sht->vy0 = vy0;
	
	if (sht->height >= 0) /* �����ͼ�㲻��һ������ͼ�� */
	{ 
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
	return;
}

/* �ͷ�ĳһͼ��ĺ��� */
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht)
{
	if (sht->height >= 0) /* ������ͼ������ʾ�� */
	{			
		sheet_updown(ctl, sht, -1); /* �޸���߶�Ϊ-1 �������� */
	}
	sht->flags = 0; /* ����Ϊδʹ��״̬ */
	return;
}

