.section .text
.globl _start

_start:
    lgdt GDT_POINTER(%rip)
    lidt IDT_POINTER(%rip)

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    movq $0x7c00, %rsp

    movq $0x101000, %rax
    movq %rax, %cr3
    movq switch_seg(%rip), %rax
    pushq $0x08
    pushq %rax
    lretq

// 64-bit mode code

switch_seg:
    .quad entry64

entry64:
    movq $0x10, %rax
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %gs
    movq %rax, %ss
    movq $0xffff800000007c00, %rsp

    movq start_kernel_addr(%rip), %rax
    pushq $0x08
    pushq %rax
    lretq

start_kernel_addr:
    .quad start_kernel

.align 8

.org 0x1000
_PML4E:
    .quad   0x102007
    .fill   255, 8, 0
    .quad   0x102007
    .fill   255, 8, 0

.org 0x2000
_PDPTE:
    .quad   0x103003
    .fill   511, 8, 0

.org 0x3000
_PDE:
    .quad    0x000083
    .quad    0x200083
    .quad    0x400083
    .quad    0x600083
    .quad    0x800083
    .quad    0xe0000083
    .quad    0xe0200083
    .quad    0xe0400083
    .quad    0xe0600083
    .quad    0xe0800083
    .quad    0xe0a00083
    .quad    0xe0c00083
    .quad    0xe0e00083
    .fill    499, 8, 0

.section .data
.globl GDT_TABLE
GDT_TABLE:
    .quad    0x0000000000000000             /*0    NULL     descriptor                   00*/
    .quad    0x0020980000000000             /*1    KERNEL   Code    64-bit    Segment    08*/
    .quad    0x0000920000000000             /*2    KERNEL   Data    64-bit    Segment    10*/
    .quad    0x0020f80000000000             /*3    USER     Code    64-bit    Segment    18*/
    .quad    0x0000f20000000000             /*4    USER     Data    64-bit    Segment    20*/
    .quad    0x00cf9a000000ffff             /*5    KERNEL   Code    32-bit    Segment    28*/
    .quad    0x00cf92000000ffff             /*6    KERNEL   Data    32-bit    Segment    30*/
    .fill    10, 8, 0                       /*8 ~ 9    TSS (jmp one segment <7>) in long-mode 128-bit 40*/
GDT_END:

GDT_POINTER:
GDT_LIMIT:
    .word   GDT_END - GDT_TABLE - 1
GDT_BASE:
    .quad   GDT_TABLE

.globl IDT_TABLE
IDT_TABLE:
    .fill  512, 8, 0
IDT_END:

IDT_POINTER:
IDT_LIMIT:
    .word   IDT_END - IDT_TABLE - 1
IDT_BASE:
    .quad   IDT_TABLE

.globl TSS64_TABLE
TSS64_TABLE:
    .fill   13,8,0
TSS64_END:

TSS64_POINTER:
TSS64_LIMIT:
    .word   TSS64_END - TSS64_TABLE - 1
TSS64_BASE:
    .quad   TSS64_TABLE

