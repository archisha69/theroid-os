/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: tss.h
*/

#include <common.h>

typedef struct {
    u32 prev_tss;
    u32 esp0, esp1, esp2;
    u32 ss0, ss1, ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax, ebx, ecx, edx;
    u32 esp, ebp, esi, edi;
    u32 es, cs, ss, ds, fs, gs;
    u32 ldt;

    u16 trap;
    u16 iomap_base;
} __attribute__((packed)) tss_entry_t;

extern void tss_flush();
void write_tss(int num, u16 ss0, u32 esp0);
void set_kernel_stack(u32 stack);