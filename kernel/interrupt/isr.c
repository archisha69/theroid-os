/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "isr.h"
#include "ports.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "kernel.h"
#include "hal/hal.h"
#include "hw/display.h"

isr_t interrupt_handlers[256];

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(int n, u32 handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].high_offset = high_16(handler);
}

static void load_idt() {
    idt_reg.base = (u32) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    asm volatile("lidt (%0)" : : "r" (&idt_reg));
}

static void init_pic() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void init_idt() {
    load_idt();

    set_idt_gate(0, (u32) isr0);
    set_idt_gate(1, (u32) isr1);
    set_idt_gate(2, (u32) isr2);
    set_idt_gate(3, (u32) isr3);
    set_idt_gate(4, (u32) isr4);
    set_idt_gate(5, (u32) isr5);
    set_idt_gate(6, (u32) isr6);
    set_idt_gate(7, (u32) isr7);
    set_idt_gate(8, (u32) isr8);
    set_idt_gate(9, (u32) isr9);
    set_idt_gate(10, (u32) isr10);
    set_idt_gate(11, (u32) isr11);
    set_idt_gate(12, (u32) isr12);
    set_idt_gate(13, (u32) isr13);
    set_idt_gate(14, (u32) isr14);
    set_idt_gate(15, (u32) isr15);
    set_idt_gate(16, (u32) isr16);
    set_idt_gate(17, (u32) isr17);
    set_idt_gate(18, (u32) isr18);
    set_idt_gate(19, (u32) isr19);
    set_idt_gate(20, (u32) isr20);
    set_idt_gate(21, (u32) isr21);
    set_idt_gate(22, (u32) isr22);
    set_idt_gate(23, (u32) isr23);
    set_idt_gate(24, (u32) isr24);
    set_idt_gate(25, (u32) isr25);
    set_idt_gate(26, (u32) isr26);
    set_idt_gate(27, (u32) isr27);
    set_idt_gate(28, (u32) isr28);
    set_idt_gate(29, (u32) isr29);
    set_idt_gate(30, (u32) isr30);
    set_idt_gate(31, (u32) isr31);

    set_idt_gate(IRQ0, (u32)irq0);
    set_idt_gate(IRQ1, (u32)irq1);
    set_idt_gate(IRQ2, (u32)irq2);
    set_idt_gate(IRQ3, (u32)irq3);
    set_idt_gate(IRQ4, (u32)irq4);
    set_idt_gate(IRQ5, (u32)irq5);
    set_idt_gate(IRQ6, (u32)irq6);
    set_idt_gate(IRQ7, (u32)irq7);
    set_idt_gate(IRQ8, (u32)irq8);
    set_idt_gate(IRQ9, (u32)irq9);
    set_idt_gate(IRQ10, (u32)irq10);
    set_idt_gate(IRQ11, (u32)irq11);
    set_idt_gate(IRQ12, (u32)irq12);
    set_idt_gate(IRQ13, (u32)irq13);
    set_idt_gate(IRQ14, (u32)irq14);
    set_idt_gate(IRQ15, (u32)irq15);

    init_pic();
}

char *exception_messages[] = {
        "division by 0",
        "debug",
        "non maskable interrupt",
        "breakpoint",
        "overflow",
        "out of bounds",
        "invalid opcode",
        "device unaivalable",
        "double fault",
        "coprocessor segment overrun",
        "invalid tss",
        "segment not present",
        "stack segment fault",
        "general protection fault",
        "page fault",
        "reserved",
        "x87 floating point exception",
        "alignment check",
        "machine check",
        "simd floating point exception",
        "virtualization exception",
        "control protection fault",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "hypervisor injection exception",
        "vmm communication exception",
        "security exception",
        "reserved"
};

void isr_handler(registers_t *r) {
    #ifdef DEBUG
    print_string(exception_messages[r->int_no]);
    HALT;
    #else
    panic(exception_messages[r->int_no]);
    #endif
}

void register_interrupt_handler(u8 n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t *r) {
    if (interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }

    if (r->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}