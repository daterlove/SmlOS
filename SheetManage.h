/*���ļ���������ͼ�㴦����صĺ���*/
#ifndef _SHEETMANAGE_H_
#define _SHEETMANAGE_H_

/* ͼ�����ṹ��ʼ������ */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
/* �����µ�ͼ�� */
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
/* ͼ�����ú��� */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
/* �ػ�����ͼ���ĳһ�������� */
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1);
/* ͼ��߶��趨���� */
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);
/* ˢ��ĳͼ�㻺�����е�һ���������� ,ע�⣡�˴�������4������������ǻ������е����� */
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1);
/* �ƶ�ͼ��ĺ��� */
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0);
/* �ͷ�ĳһͼ��ĺ��� */
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht);

#endif
