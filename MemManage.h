#ifndef _MEMMANAGE_H_
#define _MEMMANAGE_H_
/* �������ú����CR0��NW��CDλ,���ø��ٻ����� */
#define CR0_CACHE_DISABLE	0x60000000	
/* FREEINFO�ṹ������ ��Լ��32KB */
#define MEMMAN_FREES		4090	

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);

/* �ڴ���亯��  4KΪ��λ */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
/* �ڴ��ͷź���	4KΪ��λ */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);
#endif
