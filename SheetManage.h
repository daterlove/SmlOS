/*该文件定义了与图层处理相关的函数*/
#ifndef _SHEETMANAGE_H_
#define _SHEETMANAGE_H_

/* 图层管理结构初始化函数 */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
/* 创建新的图层 */
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
/* 图层设置函数 */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
/* 重画所有图层的某一矩形区域 */
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1);
/* 图层高度设定函数 */
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);
/* 刷新某图层缓冲区中的一个矩形区域 ,注意！此处给出的4个坐标参数都是缓冲区中的坐标 */
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1);
/* 移动图层的函数 */
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0);
/* 释放某一图层的函数 */
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht);

#endif
