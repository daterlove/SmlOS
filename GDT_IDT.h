#ifndef  _GDT_IDT_H_
#define  _GDT_IDT_H_

#define ADR_IDT			0x0026f800		/* IDT�׵�ַ */
#define LIMIT_IDT		0x000007ff		/* IDT�޳� */
#define ADR_GDT			0x00270000		/* GDT�׵�ַ */
#define LIMIT_GDT		0x0000ffff		/* GDT�޳� */
#define ADR_BOTPAK		0x00280000		/* �ں˴����׵�ַ */
#define LIMIT_BOTPAK	0x0007ffff		/* �ں˴����޳�(��512KB) */
#define AR_DATA32_RW	0x4092			/* 32λ���ݶ�����ֵ */
#define AR_CODE32_ER	0x409a			/* 32λ���������ֵ */
#define AR_INTGATE32	0x008e			/* 32λ������������ֵ */


/*
	limit_low	���޳���0~15bit		base_low	�λ�ַ��0~15bit
	base_mid	�λ�ַ��16~23bit	access_right(0~3bit TYPE�ֶ�, 4bit S�ֶ� 5~6 DPL�ֶ� 7bit P�ֶ�)
	limit_high	���޳���16~19bit+AVL+D/B+G��	base_high	�λ�ַ��24~31bit
 */
struct SEGMENT_DESCRIPTOR 
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

/* ������������Ľṹ�� */
/*
	���������������������,ֻ����Щ�ֶ���Щ����
	offset_low	����ƫ�Ƶ�ַ�ĵ�16bit	selector ��ѡ����
	dw_count	��������, ֻ�ǵ���������Ч
	access_right	0~3bit:TYPE�ֶ�, 4bit:S�ֶ�, 5~6bit:DPL�ֶ�, 7bit:P�ֶ�
	offset_high	����ƫ�Ƶ�ַ�ĸ�16bit
 */
struct GATE_DESCRIPTOR 
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};


/* ��ʼ��GDT, IDT */
void init_gdtidt(void);

/*���ö���������sd ���������ṹָ�룬limit ���޳���base �λ�ַ��ar ������������*/
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);

/*������������ ��gd ���������ṹָ�룬offset �������ƫ�ƣ�selector ��ѡ���ӣ�ar ������������*/
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif
