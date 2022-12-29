/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: paging.c
*/

#include <vga.h>
#include <kheap.h>
#include <stdio.h>
#include <paging.h>
#include <stdlib.h>
#include <string.h>

#include <hal/panic.h>

#define INDEX_FROM_BIT(a) (a / 32)
#define OFF_FROM_BIT(a) (a % 32)

extern void copy_page_physical(u32, u32);
extern heap_t *kheap;

pagedir_t *kernel_dir = 0;
pagedir_t *crt_dir = 0;

u32 *frames;
u32 nframes;

extern u32 placement_addr;

static void set_frame(u32 addr) {
    u32 frame = addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFF_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(u32 addr) {
    u32 frame = addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFF_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static u32 test_frame(u32 addr) {
    u32 frame = addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFF_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static u32 first_frame() {
    u32 i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] != 0xFFFFFFFF) {
            for (j = 0; j < 32; j++) {
                u32 test = 0x1 << j;
                if (!(frames[i] & test)) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }
}

void alloc_frame(page_t *page, int kernel, int writable) {
    if (page->frame != 0) {
        return;
    } else {
        u32 idx = first_frame();
        if (idx == (u32) - 1) {
            panic("no free frames");
        }
        set_frame(idx * 0x1000);
        page->present = 1;
        page->rw = (writable) ? 1 : 0;
        page->user = (kernel) ? 0 : 1;
        page->frame = idx;
    }
}

void free_frame(page_t *page) {
    u32 frame;
    if (!(frame = page->frame)) {
        return;
    } else {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void init_paging() {
    u32 mem_end = 0x1000000;

    nframes = mem_end / 0x1000;
    frames = (u32 *) kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    u32 addr;
    kernel_dir = (pagedir_t *) kmalloc_a(sizeof(pagedir_t));
    memset(kernel_dir, 0, sizeof(pagedir_t));
    kernel_dir->addr = (u32) kernel_dir->tab_phy;

    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SZ; i += 0x1000) {
        get_page(i, 1, kernel_dir);
    }

    i = 0;
    while (i < placement_addr + 0x1000) {
        alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
        i += 0x1000;
    }

    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SZ; i += 0x1000) {
        alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
    }

    register_interrupt_handler(14, pgf);
    switch_page_dir(kernel_dir);

    kheap = mkheap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SZ, 0xCFFFF000, 0, 0);

    crt_dir = clone_dir(kernel_dir);
    switch_page_dir(crt_dir);
}

void switch_page_dir(pagedir_t *dir) {
    crt_dir = dir;
    asm volatile("mov %0, %%cr3" :: "r"(dir->addr));
    u32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

page_t *get_page(u32 addr, int make, pagedir_t *dir) {
    addr /= 0x1000;
    u32 tab_idx = addr / 1024;
    if (dir->tables[tab_idx]) {
        return &dir->tables[tab_idx]->pages[addr % 1024];
    } else if (make) {
        u32 tmp;
        dir->tables[tab_idx] = (pagetab_t *) kmalloc_ap(sizeof(pagetab_t), &tmp);
        memset(dir->tables[tab_idx], 0, 0x1000);
        dir->tab_phy[tab_idx] = tmp | 0x7;
        return &dir->tables[tab_idx]->pages[addr % 1024];
    } else {
        return 0;
    }
}

void pgf(regs_t regs) {
    u32 addr;
    asm volatile("mov %%cr2, %0" : "=r" (addr));

    int present = !(regs.err_code & 0x01);
    int rw = regs.err_code & 0x2;
    int us = regs.err_code & 0x4;
    int reserved = regs.err_code & 0x8;
    int id = regs.err_code & 0x10;

    printf("page fault at %x (", addr);
    if (present) printf("present");
    if (rw) printf(" read-only");
    if (us) printf(" user-mode");
    if (reserved) printf(" reserved");
    printf(") instruction address: %x", id);
}

static pagetab_t *clone_table(pagetab_t *src, u32 *addr) {
    pagetab_t *tab = (pagetab_t *) kmalloc_ap(sizeof(pagetab_t), addr);
    memset(tab, 0, sizeof(pagedir_t));

    for (int i = 0; i < 1024; i++) {
        if (src->pages[i].frame) {
            alloc_frame(&tab->pages[i], 0, 0);

            if (src->pages[i].present)  tab->pages[i].present = 1;
            if (src->pages[i].rw)       tab->pages[i].rw = 1;
            if (src->pages[i].user)     tab->pages[i].user = 1;
            if (src->pages[i].accessed) tab->pages[i].accessed = 1;
            if (src->pages[i].dirty)    tab->pages[i].dirty = 1;

            copy_page_physical(src->pages[i].frame * 0x1000, tab->pages[i].frame * 0x1000);
        }
    }

    return tab;
}

pagedir_t *clone_dir(pagedir_t *src) {
    u32 phy;
    pagedir_t *dir = (pagedir_t *) kmalloc_ap(sizeof(pagedir_t), &phy);
    memset(dir, 0, sizeof(pagedir_t));

    u32 off = (u32) dir->tab_phy - (u32) dir;
    dir->addr = phy + off;

    for (int i = 0; i < 1024; i++) {
        if (!src->tables[i]) {
            continue;
        }

        if (kernel_dir->tables[i] == src->tables[i]) {
            dir->tables[i] = src->tables[i];
            dir->tab_phy[i] = src->tab_phy[i];
        } else {
            u32 phy;
            dir->tables[i] = clone_table(src->tables[i], &phy);
            dir->tab_phy[i] = phy | 0x07;
        }
    }

    return dir;
}