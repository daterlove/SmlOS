#ifndef  _GDT_IDT_H_
#define  _GDT_IDT_H_

#define ADR_IDT			0x0026f800		/* IDT首地址 */
#define LIMIT_IDT		0x000007ff		/* IDT限长 */
#define ADR_GDT			0x00270000		/* GDT首地址 */
#define LIMIT_GDT		0x0000ffff		/* GDT限长 */
#define ADR_BOTPAK		0x00280000		/* 内核代码首地址 */
#define LIMIT_BOTPAK	0x0007ffff		/* 内核代码限长(共512KB) */
#define AR_DATA32_RW	0x4092			/* 32位数据段属性值 */
#define AR_CODE32_ER	0x409a			/* 32位代码段属性值 */
#define AR_INTGATE32	0x008e			/* 32位门描述符属性值 */


/*
	limit_low	段限长的0~15bit		base_low	段基址的0~15bit
	base_mid	段基址的16~23bit	access_right(0~3bit TYPE字段, 4bit S字段 5~6 DPL字段 7bit P字段)
	limit_high	段限长的16~19bit+AVL+D/B+G域	base_high	段基址的24~31bit
 */
struct SEGMENT_DESCRIPTOR 
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

/* 存放门描述符的结构体 */
/*
	门描述符与段描述符类似,只是有些字段有些许差别
	offset_low	过程偏移地址的低16bit	selector 段选择子
	dw_count	参数个数, 只是调用门中有效
	access_right	0~3bit:TYPE字段, 4bit:S字段, 5~6bit:DPL字段, 7bit:P字段
	offset_high	过程偏移地址的高16bit
 */
struct GATE_DESCRIPTOR 
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};


/* 初始化GDT, IDT */
void init_gdtidt(void);

/*设置段描述符，sd 段描述符结构指针，limit 段限长，base 段基址，ar 段描述符属性*/
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);

/*设置门描述符 ，gd 门描述符结构指针，offset 过程入口偏移，selector 段选择子，ar 门描述符属性*/
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif
