/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: idt.c
*/

#include <io.h>
#include <idt.h>
#include <string.h>
#include <stdlib.h>

#define IDT_ENTRY_NUM 256

extern void idt_flush(u32);

void init_gdt();

idt_entry_t idt_entries[IDT_ENTRY_NUM];
idt_ptr_t idt_ptr;

static void set_idt_gate(u8 num, u32 base, u16 sel, u8 flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags;
}

void init_idt() {
    idt_ptr.lim  = sizeof(idt_entry_t) * IDT_ENTRY_NUM - 1;
    idt_ptr.base = (u32) &idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t) * IDT_ENTRY_NUM);

    //remap interrupt request table
    outb(PIC_MASTER_CMD, 0x11);
    outb(PIC_SLAVE_CMD, 0x11);
    outb(PIC_MASTER_DAT, 0x20);
    outb(PIC_SLAVE_DAT, 0x28);
    outb(PIC_MASTER_DAT, 0x04);
    outb(PIC_SLAVE_DAT, 0x02);
    outb(PIC_MASTER_DAT, 0x01);
    outb(PIC_SLAVE_DAT, 0x01);
    outb(PIC_MASTER_DAT, 0x0);
    outb(PIC_SLAVE_DAT, 0x0);

    set_idt_gate(0, (u32) isr0, 0x08, 0x8E);
    set_idt_gate(1, (u32) isr1, 0x08, 0x8E);
    set_idt_gate(2, (u32) isr2, 0x08, 0x8E);
    set_idt_gate(3, (u32) isr3, 0x08, 0x8E);
    set_idt_gate(4, (u32) isr4, 0x08, 0x8E);
    set_idt_gate(5, (u32) isr5, 0x08, 0x8E);
    set_idt_gate(6, (u32) isr6, 0x08, 0x8E);
    set_idt_gate(7, (u32) isr7, 0x08, 0x8E);
    set_idt_gate(8, (u32) isr8, 0x08, 0x8E);
    set_idt_gate(9, (u32) isr9, 0x08, 0x8E);
    set_idt_gate(10, (u32) isr10, 0x08, 0x8E);
    set_idt_gate(11, (u32) isr11, 0x08, 0x8E);
    set_idt_gate(12, (u32) isr12, 0x08, 0x8E);
    set_idt_gate(13, (u32) isr13, 0x08, 0x8E);
    set_idt_gate(14, (u32) isr14, 0x08, 0x8E);
    set_idt_gate(15, (u32) isr15, 0x08, 0x8E);
    set_idt_gate(16, (u32) isr16, 0x08, 0x8E);
    set_idt_gate(17, (u32) isr17, 0x08, 0x8E);
    set_idt_gate(18, (u32) isr18, 0x08, 0x8E);
    set_idt_gate(19, (u32) isr19, 0x08, 0x8E);
    set_idt_gate(20, (u32) isr20, 0x08, 0x8E);
    set_idt_gate(21, (u32) isr21, 0x08, 0x8E);
    set_idt_gate(22, (u32) isr22, 0x08, 0x8E);
    set_idt_gate(23, (u32) isr23, 0x08, 0x8E);
    set_idt_gate(24, (u32) isr24, 0x08, 0x8E);
    set_idt_gate(25, (u32) isr25, 0x08, 0x8E);
    set_idt_gate(26, (u32) isr26, 0x08, 0x8E);
    set_idt_gate(27, (u32) isr27, 0x08, 0x8E);
    set_idt_gate(28, (u32) isr28, 0x08, 0x8E);
    set_idt_gate(29, (u32) isr29, 0x08, 0x8E);
    set_idt_gate(30, (u32) isr30, 0x08, 0x8E);
    set_idt_gate(31, (u32) isr31, 0x08, 0x8E);

    set_idt_gate(32, (u32) irq0, 0x08, 0x8E);
    set_idt_gate(33, (u32) irq1, 0x08, 0x8E);
    set_idt_gate(34, (u32) irq2, 0x08, 0x8E);
    set_idt_gate(35, (u32) irq3, 0x08, 0x8E);
    set_idt_gate(36, (u32) irq4, 0x08, 0x8E);
    set_idt_gate(37, (u32) irq5, 0x08, 0x8E);
    set_idt_gate(38, (u32) irq6, 0x08, 0x8E);
    set_idt_gate(39, (u32) irq7, 0x08, 0x8E);
    set_idt_gate(40, (u32) irq8, 0x08, 0x8E);
    set_idt_gate(41, (u32) irq9, 0x08, 0x8E);
    set_idt_gate(42, (u32) irq10, 0x08, 0x8E);
    set_idt_gate(43, (u32) irq11, 0x08, 0x8E);
    set_idt_gate(44, (u32) irq12, 0x08, 0x8E);
    set_idt_gate(45, (u32) irq13, 0x08, 0x8E);
    set_idt_gate(46, (u32) irq14, 0x08, 0x8E);
    set_idt_gate(47, (u32) irq15, 0x08, 0x8E);
    set_idt_gate(128, (u32) isr128, 0x08, 0x8E);

    idt_flush((u32) &idt_ptr);
}