#ifndef _VARIABLETYPE_H
#define _VARIABLETYPE_H
/*启动区信息 结构体*/
struct BOOTINFO 
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

/* 鼠标缓冲区结构 
	buf[]是缓冲区,存放鼠标发送过来的3字节数据
	phase用来标识接收到鼠标数据的第几步
	x, y存放鼠标位移量	bth记录按键信息
 */
 struct MOUSE_DEC 
{
	unsigned char buf[3], phase;
	int x, y, btn;
};

#endif
