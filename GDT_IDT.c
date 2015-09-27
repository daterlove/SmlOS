#include "common.h"

/* ��ʼ��GDT, IDT */
void init_gdtidt(void)
{
	/* GDT��λ�ý�����IDT֮�� */
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
	
	int i;

	/* GDT��ʼ�� */
	for (i = 0; i < 8192; i++) 
	{
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	/* ���Ϊ0���������ǿ������� ��IntelҪ������������õ� */
	
	/* �ں����ݶΣ�32λ�ɶ�д���ݶ������� ���޳�4G-1 �λ�ַ0  DPL = 0*/
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);	
	
	/* �ں˴���Σ�32λ�ɶ���ִ�д���������� ���޳�512KB  �λ�ַ0x280000 DPL = 0*/
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);	
																
	/* ����GDT */															
	load_gdtr(0xffff, 0x00270000);							

	/* IDT��ʼ�� */
	for (i = 0; i < 256; i++) 
	{
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(0x7ff, 0x0026f800);//����IDT	

	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);	/* ����INT 0x20�жϵ��������� */
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);	/* ����INT 0x21�жϵ��������� */
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);	/* ����INT 0x27�жϵ��������� */
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);	/* ����INT 0x2c�жϵ��������� */

	return;
}


/*���ö���������sd ���������ṹָ�룬limit ���޳���base �λ�ַ��ar ������������*/
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	/* ����޳�����1M (20bit�Ķ��޳������ֽ�Ϊ��λ���ֻ�ܱ�ʾ1M���ڴ�)*/
	if (limit > 0xfffff) 
	{
		/* ����������һ��Gλ�� ��G = 1���ʾ���޳�����4KBΪ��λ����G = 0 ���ʾ���޳������ֽ�Ϊ��λ�� */
		ar |= 0x8000; /* G_bit = 1 */	
		
		/* �������޳���ֵ������4K */									
		limit /= 0x1000;				
	}
	
	/* ����ֵ������Ӧ������ �ɲο���������Ķ�������ͼ */
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}


/*������������ ��gd ���������ṹָ�룬offset �������ƫ�ƣ�selector ��ѡ���ӣ�ar ������������*/
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
