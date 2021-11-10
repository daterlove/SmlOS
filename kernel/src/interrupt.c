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

void do_divide_error(unsigned long rsp, unsigned long error_code)
{
    unsigned long * p = NULL;
    p = (unsigned long *)(rsp + 0x98);
    int32* addr = (int *)0xffff800000a00000;
    char buf[90] = {0};
    snprintf(buf, sizeof(buf), "interrupt: divide error, rsp:%p, rip:%p, error_code:%ld",
        rsp, *p, error_code);

    string_print_color(addr, 1440, 101, 224, buf, 0x00000000);
}

void idt_init()
{
    int i;
    for (i = 0; i < IDT_ENTRIES; i++)
    {
        set_gate(i, GATE_INTERRUPT, divide_error, 0, 0, __KERNEL_CS);
    }
    struct desc_ptr idt_ptr;
    idt_ptr.size = IDT_ENTRIES * sizeof(struct gate_struct64_t) - 1;
    idt_ptr.address = (uint64)g_idt_table;

    native_load_idt(&idt_ptr);

}