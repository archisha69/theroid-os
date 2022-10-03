/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "mem.h"
#include "isr.h"
#include "rand.h"
#include "stdio.h"
#include "stdlib.h"
#include "ports.h"
#include "types.h"
#include "string.h"
#include "timer.h"
#include "kernel.h"

#include "hal/hal.h"
#include "hw/drive.h"
#include "hw/display.h"
#include "hw/keyboard.h"

#define GDTBASE	0x00000800

typedef struct {
    u16 lim0_15;
    u16 base0_15;
    u8 base16_23;
    u8 access;
    u8 granularity;
    u8 base24_31;
} __attribute__((packed)) gdt_t;

typedef struct {
    u16 gdt_size;
    gdt_t *gdt_addr;
} __attribute__((packed)) gdt_ptr;

typedef struct {
	u16 link;
    u16 res0, res1, res2, res3, res4, res5, res6, \
        res7, res8, res9, res10, res11;
	u32 esp0, esp1, esp2;
	u16 ss0, ss1, ss2;
	u32 cr3, eip, eflags;
	u32 eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, cs, ds, fs, gs, ss;
	u16 ldtr;
	u16 iopb;
	u32 ssp;
} __attribute__((packed)) tss_t;

static gdt_t gdt[5];
static gdt_ptr gdtptr;

extern void load_gdt(gdt_t*);

static void fill_gdt_entry(u8 n, u16 l015, u16 b015, u8 b1623, u8 access, u8 gr, u8 b2431) {
    gdt[n].lim0_15 = l015;
    gdt[n].base0_15 = b015;
    gdt[n].base16_23 = b1623;
    gdt[n].access = access;
    gdt[n].granularity = gr;
    gdt[n].base24_31 = b2431;
}

void start_kernel() {
    clear_screen();

    #ifdef DEBUG
    printf("Initializing global descriptor table\n");
    #endif

    fill_gdt_entry(0, 0, 0, 0, 0, 0, 0);
    fill_gdt_entry(1, 0xffff, 0, 0, 0b10011010, 0b11001111, 0);
    fill_gdt_entry(2, 0xffff, 0, 0, 0b10010010, 0b11001111, 0);
    fill_gdt_entry(3, 0xffff, 0, 0, 0b11111010, 0b11001111, 0);
    fill_gdt_entry(4, 0xffff, 0, 0, 0b11110010, 0b11001111, 0);

    gdtptr.gdt_size = sizeof(gdt) - 1;
    gdtptr.gdt_addr = (gdt_t*) &gdt;
    load_gdt((gdt_t*) &gdtptr);

    #ifdef DEBUG
    printf("Initializing interrupt descriptor table\n");
    #endif
    init_idt();

    #ifdef DEBUG
    printf("Enabling external interrupts\n");
    #endif
    INT_START;

    #ifdef DEBUG
    printf("Initializing timer (IRQ 0)\n");
    #endif
    init_timer(1);

    #ifdef DEBUG
    printf("[%u] Initializing dynamic memory\n", gettick());
    #endif
    init_dynamic_mem();

    #ifdef DEBUG
    printf("[%u] Testing dynamic memory\n", gettick());
    void *ptr = malloc(10);
    if (ptr == nullptr) panic("malloc: failed to allocate memory");
    ptr = realloc(ptr, 20);
    if (ptr == nullptr) panic("realloc: failed to reallocate memory");
    void *ptr1 = calloc(2, 10);
    if (ptr1 == nullptr) panic("calloc: failed to allocate memory");
    free(ptr);
    free(ptr1);
    #endif

    #ifdef DEBUG
    printf("[%u] Initializing keyboard (IRQ 1)\n", gettick());
    #endif
    init_keyboard();

    printf("Welcome to theroid os!\ntype help for a command list\n>> ");
}

void panic(char *message) {
    clear_screen();
    printf("----beginning of kernel panic----\n\n[%u] %s\n\n-------end of kernel panic-------\n", gettick(), message);
    INT_STOP;
    HALT;
}