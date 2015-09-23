#ifndef _KEY_MOUSE_H
#define _KEY_MOUSE_H

/*端口常量*/
#define PORT_KEYDAT				0x0060			/* i8042的数据端口号 */
#define PORT_KEYSTA				0x0064			/* i8042的状态端口号 */
#define PORT_KEYCMD				0x0064			/* i8042的命令端口号 */	
/* 事实上状态端口和命令端口是同一个端口，只是定义了两个不同的符号常量名,做区分 */


#define KEYSTA_SEND_NOTREADY	0x02			/*这个符号常量用来判断i8042的输入缓冲区是否满 */
#define KEYCMD_WRITE_MODE		0x60			/* 发送给i8042的命令 下面有详细解释 */
#define KBC_MODE				0x47			/* 将被设置为i8042的控制寄存器的值 */

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4	/* 该命令可以允许鼠标向主机发送数据包 */

/* 等待键盘控制电路准备完毕 */
void wait_KBC_sendready(void);
/* 初始化键盘控制电路 */
void Init_Keyboard(void);
/* 激活鼠标 */
void Enable_Mouse(void);
/* 接收鼠标数据 */
int Mouse_Decode(struct MOUSE_DEC *mdec, unsigned char dat);


#endif
