/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: gdt.c
*/

#include <gdt.h>
#include <tss.h>

#define GDT_ENTRY_NUM 6

extern void gdt_flush(u32);
extern tss_entry_t tss;


gdt_entry_t gdt_entries[GDT_ENTRY_NUM];
gdt_ptr_t gdt_ptr;

void set_gdt_gate(u8 num, u32 base, u32 lim, u8 access, u8 gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].lim_low     = (lim & 0xFFFF);
    gdt_entries[num].granularity = (lim >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRY_NUM) - 1;
    gdt_ptr.base = (u32) &gdt_entries;

    set_gdt_gate(0, 0, 0, 0,0);                 //null segment
    set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); //kernel code segment
    set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); //kernel data segment
    set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); //usermode code segment
    set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); //usermode data segment
    write_tss(5, 0x10, 0x0);                                 //task state segment

    gdt_flush((u32) &gdt_ptr);
    tss_flush();
}