/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: paging.h
*/

#include <isr.h>

#define PAGE_DIR_SIZE 1024
#define PAGE_TAB_SIZE 1024

typedef struct {
    u32 present  : 1;
    u32 rw       : 1;
    u32 user     : 1;
    u32 accessed : 1;
    u32 dirty    : 1;
    u32 unused   : 7;
    u32 frame    : 20;
} page_t;

typedef struct {
    page_t pages[PAGE_TAB_SIZE];
} pagetab_t;

typedef struct {
    pagetab_t *tables[PAGE_DIR_SIZE];
    u32 tab_phy[PAGE_DIR_SIZE];
    u32 addr;
} pagedir_t;

extern pagedir_t *kernel_dir;

void init_paging();
void switch_page_dir(pagedir_t *dir);
page_t *get_page(u32 addr, int make, pagedir_t *dir);
void pgf(regs_t regs);
void alloc_frame(page_t *page, int kernel, int writable);
void free_frame(page_t *page);