#include "common.h"

/* 初始化GDT, IDT */
void init_gdtidt(void)
{
	/* GDT的位置紧随在IDT之后 */
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
	
	int i;

	/* GDT初始化 */
	for (i = 0; i < 8192; i++) 
	{
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	/* 编号为0的描述符是空描述符 是Intel要求必须这样设置的 */
	
	/* 内核数据段，32位可读写数据段描述符 段限长4G-1 段基址0  DPL = 0*/
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);	
	
	/* 内核代码段，32位可读可执行代码段描述符 段限长512KB  段基址0x280000 DPL = 0*/
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);	
																
	/* 加载GDT */															
	load_gdtr(0xffff, 0x00270000);							

	/* IDT初始化 */
	for (i = 0; i < 256; i++) 
	{
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(0x7ff, 0x0026f800);//加载IDT	

	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);	/* 设置INT 0x20中断的门描述符 */
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);	/* 设置INT 0x21中断的门描述符 */
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);	/* 设置INT 0x27中断的门描述符 */
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);	/* 设置INT 0x2c中断的门描述符 */

	return;
}


/*设置段描述符，sd 段描述符结构指针，limit 段限长，base 段基址，ar 段描述符属性*/
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	/* 如果限长大于1M (20bit的段限长若以字节为单位最多只能表示1M的内存)*/
	if (limit > 0xfffff) 
	{
		/* 描述符中有一个G位， 若G = 1则表示段限长是以4KB为单位，若G = 0 则表示段限长是以字节为单位的 */
		ar |= 0x8000; /* G_bit = 1 */	
		
		/* 调整段限长的值即除以4K */									
		limit /= 0x1000;				
	}
	
	/* 将各值填入相应的域中 可参考上面给出的段描述符图 */
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}


/*设置门描述符 ，gd 门描述符结构指针，offset 过程入口偏移，selector 段选择子，ar 门描述符属性*/
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
