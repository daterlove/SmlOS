/*该文件定义了与图层处理相关的函数*/
#ifndef _SHEETMANAGE_H_
#define _SHEETMANAGE_H_

/* 图层管理结构初始化函数 */
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
/* 创建新的图层 */
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
/* 图层设置函数 */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
/* 图层高度设定函数 */
void sheet_updown(struct SHEET *sht, int height);
/* 显示所有图层的函数 */
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
/* 移动图层的函数 */
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
/* 释放某一图层的函数 */
void sheet_free(struct SHEET *sht);

#endif
