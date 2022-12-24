/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: kheap.h
*/

#pragma once

#include <common.h>
#include <ordered_array.h>

#define KHEAP_START      0xC0000000
#define KHEAP_INITIAL_SZ 0x100000
#define HEAP_INDEX_SZ    0x20000
#define HEAP_MAGIC       0x69694200
#define HEAP_MIN_SZ      0x70000

typedef struct {
    u32 magic;
    u8 hole;
    u32 size;
} header_t;

typedef struct {
    u32 magic;
    header_t *head;
} footer_t;

typedef struct {
    ordered_array_t index;
    u32 start;
    u32 end;
    u32 max;
    u8 supervisor;
    u8 ro;
} heap_t;

u32 kmalloc_int(u32 sz, int align, u32 *phy);
u32 kmalloc_a(u32 sz);
u32 kmalloc_p(u32 sz, u32 *phy);
u32 kmalloc_ap(u32 sz, u32 *phy);
u32 kmalloc(u32 sz);

heap_t *mkheap(u32 start, u32 end, u32 max, u8 supervisor, u8 ro);
void *alloc(u32 size);
void kfree(void *p);