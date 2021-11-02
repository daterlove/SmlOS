#ifndef _LINKAGE_H_
#define _LINKAGE_H_

#define asmlinkage __attribute__((regparm(0)))
#define ____cacheline_aligned __attribute__((__aligned__(L1_CACHE_BYTES)))

#endif
