#include "common.h"
#include "interrupt.h"

struct gate_struct64_t g_idt_table[IDT_ENTRIES]  __attribute__((__section__(".data.idt"))) = { {0, 0}, };

void native_load_gdt(const struct desc_ptr *dtr)
{
    asm volatile("lgdt %0"::"m" (*dtr));
}

void native_load_idt(const struct desc_ptr *dtr)
{
    asm volatile("lidt %0"::"m" (*dtr));
}

void pack_gate(
    struct gate_struct64_t *gate,
    unsigned type,
    unsigned long func,
    unsigned dpl,
    unsigned ist,
    unsigned seg
)
{
    gate->offset_low    = PTR_LOW(func);
    gate->segment       = __KERNEL_CS;
    gate->ist           = ist;
    gate->p             = 1;
    gate->dpl           = dpl;
    gate->zero0         = 0;
    gate->zero1         = 0;
    gate->type          = type;
    gate->offset_middle = PTR_MIDDLE(func);
    gate->offset_high   = PTR_HIGH(func);
}

void set_gate(int gate_index, unsigned type, void *addr,
                 unsigned dpl, unsigned ist, unsigned seg)
{
    struct gate_struct64_t gate;

    pack_gate(&gate, type, (unsigned long)addr, dpl, ist, seg);
    memcpy(&g_idt_table[gate_index], &gate, sizeof(struct gate_struct64_t));
}
