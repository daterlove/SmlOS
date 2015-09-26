/*���ļ���������ͼ�㴦����صĺ���*/
#ifndef _SHEETMANAGE_H_
#define _SHEETMANAGE_H_

/* ͼ�����ṹ��ʼ������ */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
/* �����µ�ͼ�� */
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
/* ͼ�����ú��� */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
/* ͼ��߶��趨���� */
void sheet_updown(struct SHEET *sht, int height);
/* ��ʾ����ͼ��ĺ��� */
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
/* �ƶ�ͼ��ĺ��� */
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
/* �ͷ�ĳһͼ��ĺ��� */
void sheet_free(struct SHEET *sht);

#endif
