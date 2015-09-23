#ifndef _INTERRUPUT_H
#define _INTERRUPUT_H

/* PIC中各个端口号 */
#define PIC0_ICW1		0x0020			
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PORT_KEYDAT		0x0060		/* 8042的数据端口号 */

void Init_PIC(void);					/* 初始化PIC */
void IntHandler21(int *esp);						
void IntHandler27(int *esp);
void IntHandler2c(int *esp);

#endif
