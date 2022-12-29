/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: tss.c
*/

#include <tss.h>
#include <gdt.h>
#include <string.h>

extern void tss_flush();

tss_entry_t tss;

void write_tss(int num, u16 ss0, u32 esp0) {
    u32 base = (u32) &tss;
    u32 lim = base + sizeof(tss);

    set_gdt_gate(num, base, lim, 0x89, 0x00);
    memset(&tss, 0, sizeof(tss));

    tss.ss0  = ss0;
    tss.esp0 = esp0;

    tss.cs   = 0x0B;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
}

void set_kernel_stack(u32 stack) {
    tss.esp0 = stack;
}