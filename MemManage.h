#ifndef _MEMMANAGE_H_
#define _MEMMANAGE_H_
/* 用于设置和清除CR0的NW与CD位,设置高速缓存用 */
#define CR0_CACHE_DISABLE	0x60000000	
/* FREEINFO结构的数量 大约是32KB */
#define MEMMAN_FREES		4090	

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);

/* 内存分配函数  4K为单位 */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
/* 内存释放函数	4K为单位 */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);
#endif
