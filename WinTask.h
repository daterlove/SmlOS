#ifndef _WINTASK_H_
#define _WINTASK_H_

/* ����B(���ܲ���)*/
void task_b_main(struct SHEET *sht_back);
void task_b_main1(struct SHEET *sht_back);
/* Window����*/
void PutWinTextChar(struct SHEET *sht_back,char *s,int nXSize,int nYSize,int *nPos_CurX,int *nPos_CurY);
void task_win_main(struct SHEET *sht_back);

#endif
