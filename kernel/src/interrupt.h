#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
#include "types.h"

struct gate_struct64_t
{
    uint16 offset_low;
    uint16 segment;
    unsigned ist    : 3,
             zero0  : 5,
             type   : 5,
             dpl    : 2,
             p      : 1;
    uint16 offset_middle;
    uint32 offset_high;
    uint32 zero1;
} __attribute__((packed));

enum
{
    GATE_INTERRUPT = 0xE,
    GATE_TRAP = 0xF,
    GATE_CALL = 0xC,
    GATE_TASK = 0x5,
};

struct desc_ptr
{
    uint16 size;
    uint32 address;
} __attribute__((packed));

#define IDT_ENTRIES 256

#define GDT_ENTRY_KERNEL_CS 1
#define GDT_ENTRY_KERNEL_DS 2
#define GDT_ENTRY_DEFAULT_USER_CS 3
#define GDT_ENTRY_DEFAULT_USER_DS 4

#define __KERNEL_CS     (GDT_ENTRY_KERNEL_CS * 8)
#define __KERNEL_DS     (GDT_ENTRY_KERNEL_DS * 8)
#define __USER_DS       (GDT_ENTRY_DEFAULT_USER_DS * 8 + 3)
#define __USER_CS       (GDT_ENTRY_DEFAULT_USER_CS * 8 + 3)


#define PTR_LOW(x)      ((unsigned long long)(x) & 0xFFFF)
#define PTR_MIDDLE(x)   (((unsigned long long)(x) >> 16) & 0xFFFF)
#define PTR_HIGH(x)     ((unsigned long long)(x) >> 32)

#endif