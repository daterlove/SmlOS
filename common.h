#ifndef _COMMON_H
#define	_COMMON_H

#include <stdio.h>
#include "VariableType.h"//变量类型定义，必须放在前面，否则出错

#include "Color_Define.h"
#include "GDT_IDT.h"
#include "Graphic.h"
#include "Interruput.h"
#include "FIFO.h"
#include "Key_Mouse.h"
#include "MemManage.h"
#include "SheetManage.h"
#include "Timer.h"

#define ADR_BOOTINFO	0x00000ff0	/* 存放启动信息的地址 */
#define MEMMAN_ADDR			0x003c0000	/* MEMMAN结构将被存放在该地址处 */

extern struct FIFO8 KeyFifo, MouseFifo;

/*AsmFunc.nas 里面 的汇编函数*/
void io_hlt(void);

void io_cli(void);	// 关闭所有可屏蔽中断
void io_sti(void);	// 打开所有可屏蔽中断
void io_stihlt(void);

void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

void asm_inthandler20(void);		/* IRQ1服务程序 */
void asm_inthandler21(void);		/* IRQ1服务程序 */
void asm_inthandler27(void);		/* IRQ7服务程序 */
void asm_inthandler2c(void);		/* IRQ12服务程序 */

int io_in8(int port);				// 从port端口读入8位数据到AL中
int io_in16(int port); 				//从port端口读入16位数据到AX中
int io_in32(int port); 				// 从port端口读入32位数据到EAX中
void io_out8(int port, int data);	// 将8位的data输出到port端口
void io_out16(int port, int data);	// 将16位的data输出到port端口
void io_out32(int port, int data);	// 将32位的data输出到port端口
	
int io_load_eflags(void);			// 将EFLAGS寄存器的内容返回
void io_store_eflags(int eflags);	//	将EFLAGS寄存器的内容保存

unsigned int memtest_sub(unsigned int start, unsigned int end);//内存检查函数



#endif
