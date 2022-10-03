/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

#define KERNEL_CS 0x08
#define IDT_ENTRIES 256

typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;
} registers_t;

typedef struct {
    u16 low_offset;
    u16 sel;
    u8 always0;
    u8 flags;
    u16 high_offset;
} __attribute__((packed)) idt_gate_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_register_t;

typedef void (*isr_t)(registers_t *);

// exceptions
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// external interrupt requests
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
// extern void irq32();
// extern void irq33();
// extern void irq34();
// extern void irq35();
// extern void irq36();
// extern void irq37();
// extern void irq38();
// extern void irq39();
// extern void irq40();
// extern void irq41();
// extern void irq42();
// extern void irq43();
// extern void irq44();
// extern void irq45();
// extern void irq46();
// extern void irq47();

// #define irq0 irq32
// #define irq1 irq33
// #define irq2 irq34
// #define irq3 irq35
// #define irq4 irq36
// #define irq5 irq37
// #define irq6 irq38
// #define irq7 irq39
// #define irq8 irq40
// #define irq9 irq41
// #define irq10 irq42
// #define irq11 irq43
// #define irq12 irq44
// #define irq13 irq45
// #define irq14 irq46
// #define irq15 irq47

void register_interrupt_handler(u8 n, isr_t handler);
void set_idt_gate(int n, u32 handler);
void init_idt();