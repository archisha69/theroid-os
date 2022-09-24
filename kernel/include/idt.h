/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

#define KERNEL_CS 0x08
#define IDT_ENTRIES 256

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


void set_idt_gate(int n, u32 handler);
void load_idt();
