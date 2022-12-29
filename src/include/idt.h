/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: idt.h
*/

#pragma once

#include <common.h>

typedef struct {
    u16 base_lo;
    u16 sel;
    u8 always0;
    u8 flags;
    u16 base_hi;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 lim;
    u32 base;
} __attribute__((packed)) idt_ptr_t;

#define ISR(n) extern void isr##n()
#define IRQ(n) extern void irq##n()

ISR(0);
ISR(1);
ISR(2);
ISR(3);
ISR(4);
ISR(5);
ISR(6);
ISR(7);
ISR(8);
ISR(9);
ISR(10);
ISR(11);
ISR(12);
ISR(13);
ISR(14);
ISR(15);
ISR(16);
ISR(17);
ISR(18);
ISR(19);
ISR(20);
ISR(21);
ISR(22);
ISR(23);
ISR(24);
ISR(25);
ISR(26);
ISR(27);
ISR(28);
ISR(29);
ISR(30);
ISR(31);
ISR(128);

IRQ(0);
IRQ(1);
IRQ(2);
IRQ(3);
IRQ(4);
IRQ(5);
IRQ(6);
IRQ(7);
IRQ(8);
IRQ(9);
IRQ(10);
IRQ(11);
IRQ(12);
IRQ(13);
IRQ(14);
IRQ(15);

void init_idt();