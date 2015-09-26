#include "common.h"

/* ��ʶͼ������ʹ�÷��ų��� */
#define SHEET_USE		1

/* ͼ�����ṹ��ʼ������ */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
	/* Ϊͼ�����ṹ�����ڴ� */
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));
	if (ctl == 0) /* �������ʧ�� */
	{	
		goto err;
	}
	/* Ϊmap�����ڴ�,ע�����Ĵ�С��xsize*ysize��4K����ȡ��,����������������Ļ��ͼ����Ϣ */
	ctl->map = (unsigned char *) memman_alloc_4k(memman, xsize * ysize);
	if (ctl->map == 0) /* �������ʧ�� */
	{	
		/* �ͷ����ȷ����SHTCTL�ṹ����ڴ� */
		memman_free_4k(memman, (int) ctl, sizeof (struct SHTCTL));
		goto err;
	}
	ctl->vram = vram;		
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* һ��ͼ��Ҳû�� */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets[i].flags = 0; /* ���Ϊδʹ�� */
		ctl->sheets[i].ctl = ctl; /* ��¼���� */
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
		/* ��������sheets������ҵ�һ��δʹ�õ�ͼ�� */
		if (ctl->sheets[i].flags == 0)
		{	
			sht = &ctl->sheets[i];
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

/* ������Ļ��ĳһ�����map��Ϣ */
/* �Ӹ߶�h0~top��ͼ����ѡ��ĳЩͼ��,����sidֵ������map���� */
/* vx0,vy0�����˸���������Ͻ�����	vx1,vy1�����˸���������½����� */
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
	/* ��b��ͷ�ı���������ͼ���ڻ������е����� */
	/* ��v��ͷ�ı���������ͼ������Ļ�ϵ����� */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, sid, *map = ctl->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	
	for (h = h0; h <= ctl->top; h++) 
	{		/* ������h0��top������ͼ�� */
		sht = ctl->p_sheets[h];
		sid = sht - ctl->sheets; /* sid��ͼ����sheets�е��±� */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;		/* ����ͼ���ڻ������е����� */
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		
		if (bx0 < 0) { bx0 = 0; }	/* ���Ͻ�������� */
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }	/* ���½�������� */
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		
		for (by = by0; by < by1; by++) 
		{	
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) 
			{
				vx = sht->vx0 + bx;
				/* �����һ�������ϸ�ͼ����Ҫ��ʾ��,���ǲŻὫ��sidд��map�� */
				/* ������ߵ�ͼ����ܻḲ�����ͼ��д���sidֵ */
				if (buf[by * sht->bxsize + bx] != sht->col_inv) 
				{
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
	}
	return;
}

/* �ػ���Ļ�ϵ�ĳһ���� */
/* ����map����һ�����ֵѡ��߶ȴ�h0��h1�е�ĳЩͼ������ػ� */
/* vx0, vy1����þ�����������Ͻ����� */
/* vx1, vy1����þ�����������½����� */
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	/* ��b��ͷ�ı���������ͼ���ڻ������е����� */
	/* ��v��ͷ�ı���������ͼ������Ļ�ϵ����� */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET *sht;
	/* ���refresh�ķ�Χ�����˻��� ������ */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	
	for (h = h0; h <= h1; h++) 
	{	/* ����ͼ�� */
		sht = ctl->p_sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets;
		/* ����ͼ���ڻ������е����� */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }	/* ���Ͻ�������� */
		if (by0 < 0) { by0 = 0; }	
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }	/* ���½�������� */
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		
		for (by = by0; by < by1; by++) 
		{
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) 
			{
				vx = sht->vx0 + bx;
				/* ����������mapֵ��sid��� ���ػ� */
				if (map[vy * ctl->xsize + vx] == sid) 
				{	
					vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		}
	}
	return;
}

/* ͼ��߶��趨���� */
/* ��ʵ��,�����һ������ṹ������Ĳ�����ɾ�����㷨��ʵ�� */
/* �����㷨�����׵Ŀɲο�����һ���������ݽṹ���鼮 */
void sheet_updown(struct SHEET *sht, int height)
{
	struct SHTCTL *ctl = sht->ctl;
	int h, old = sht->height; /* ����ԭ���ĸ߶� */

	/* ����µĸ߶ȱ�����ͼ������ĸ߶Ȼ��� ������� */
	if (height > ctl->top + 1) 
	{
		height = ctl->top + 1;
	}
	/* ����¸߶�С��-1 ������Ϊ-1 ����ֻ��Ҫ��-1����ʾ���ؾ��㹻�� */
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* ����ͼ��ĸ߶� */

	/* ��p_sheets[]������(p_sheets[]����һ�����߶��������������) */
	if (old > height) 
	{	/* ԭ���ĸ߶ȱ��¸߶Ȼ�Ҫ�� */
		if (height >= 0) 
		{
			/* ���м�������� */
			/* ���ǿ�����Ϊ�߶Ⱦ���ͼ����p_sheets�����е��±� */
			for (h = old; h > height; h--) 
			{	
				ctl->p_sheets[h] = ctl->p_sheets[h - 1];
				ctl->p_sheets[h]->height = h;		/* ����ԭ��p_sheets[]��ָ���ͼ��ĸ߶� */
												/* ������ԭ����ͼ����p_sheets[]�е��±� */
			}
			ctl->p_sheets[height] = sht;			/* �����µ�ͼ��ָ�� */
			/* ����map��Ϣ���ػ� */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} 
		else 
		{	/* ����ԭ��ͼ�� */
			/* ���ԭ����ͼ�㲻�����ϲ��ͼ�� */
			if (ctl->top > old) 
			{	
				/* ������Ľ����� */
				for (h = old; h < ctl->top; h++)
				{
					ctl->p_sheets[h] = ctl->p_sheets[h + 1];
					ctl->p_sheets[h]->height = h;	/* ����ԭ��p_sheets[]��ָ���ͼ��ĸ߶� */
				}
			}
			ctl->top--; /* ������ʾ�е�ͼ�������һ��, �������ϲ��ͼ��߶ȼ�һ */
			/* ����map��Ϣ���ػ� */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
		}
	} 
	else if (old < height) 
	{	/* ԭ���ĸ߶ȱ��¸߶Ȼ�ҪС */
		if (old >= 0) 
		{			/* ԭ����ͼ������������ص� */
			/* ���м������ȥ */
			for (h = old; h < height; h++) 
			{
				ctl->p_sheets[h] = ctl->p_sheets[h + 1];
				ctl->p_sheets[h]->height = h;		/* ����ԭ��p_sheets[]��ָ���ͼ��ĸ߶� */
			}
			ctl->p_sheets[height] = sht;
			
		} 
		else 
		{	/* ԭ����ͼ�������ص� */
			for (h = ctl->top; h >= height; h--) 
			{
				ctl->p_sheets[h + 1] = ctl->p_sheets[h];
				ctl->p_sheets[h + 1]->height = h + 1;		/* ����ԭ��p_sheets[]��ָ���ͼ��ĸ߶� */
			}
			ctl->p_sheets[height] = sht;		/* �����µ�ͼ��ָ�� */
			ctl->top++; /* ����������һ��Ҫ��ʾ��ͼ��,�������ϲ��ͼ��߶���Ҫ��һ */
		}
		/* ����map��Ϣ���ػ� */
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
	}
	return;
}

/* ˢ��ĳͼ�㻺�����е�һ���������� */
/* ע�⣡�˴�������4������������ǻ������е����� */
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	/* ���ͼ�㲻�����ص� */
	if (sht->height >= 0) 
	{ 
		/* �����ʵ�����겢����sheet_refreshsub�Ը���������ػ� */
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
	return;
}

/* �ƶ�ͼ��ĺ��� */
/* 
	sht����Ҫ���ƶ���ͼ��
	vx0,vy1�Ǳ��ƶ�ͼ���µ����Ͻ�����
 */
void sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
	struct SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;	/* ����ͼ��ԭ�������� */
	sht->vx0 = vx0;			/* ����ͼ���µ����� */
	sht->vy0 = vy0;
	
	/* �����ͼ�㲻��һ������ͼ�� */
	if (sht->height >= 0) 
	{ 
		/* ���޸�map��Ϣ �ٽ����ػ� */
		/* ������൱�ڽ�map��(old_vx0, old_vy0)(old_vx0 + sht->bxsize, old_vy0 + sht->bysize)�����
		   ����������������,ע��:��ʱshtͼ��������Ѿ��޸���,���Ҵ˴������ĸ߶�Ϊ0.Ҳ����˵�Ӹ߶�Ϊ0
		   ��top������ͼ�㶼���ж��Ƿ���Ҫ���Լ���sidֵд��map�и�����
		   */
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		/* ����һ���������,��ʱ�����ĸ߶���ͼ�㱾��ĸ߶� ����Ϊʲô��ο�����P213�м��һ�λ� */
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		/* �޸���map֮�����map�е�ֵ���ػ� */
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
	return;
}

/* �ͷ�ĳһͼ��ĺ��� */
void sheet_free(struct SHEET *sht)
{
	if (sht->height >= 0)
	{		/* ������ͼ������ʾ�� */
		sheet_updown(sht, -1);  /* �޸���߶�Ϊ-1 �������� */
	}
	sht->flags = 0; /* ����Ϊδʹ��״̬ */
	return;

}

