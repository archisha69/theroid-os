/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: pmm.c
*/

#include <pmm.h>
#include <stdio.h>
#include <string.h>

extern u32 end;

u8 *bitmap = (u8 *) (&end);
u8 *mem_start;
u32 total_blocks;
u32 bitmap_size;

void init_pmm(u32 sz) {
    total_blocks = sz / BLOCK_SIZE;
    bitmap_size = total_blocks / BLOCKS_PER_BUCKET;

    if (bitmap_size * BLOCKS_PER_BUCKET < total_blocks) {
        bitmap_size++;
    }
    memset(bitmap, 0, bitmap_size);
    mem_start = (u8 *) BLK_ALIGN(((u32) (bitmap + bitmap_size)));

    printf("mem size:        %u mb\n", sz / 1048576);
    printf("mem start:       %x\n", mem_start);
    printf("mem end:         %x\n", mem_start + sz);
    printf("total blocks:    %u\n", total_blocks);
    printf("bitmap addr:     %x\n", bitmap);
    printf("bitmap size:     %u", bitmap_size);
}

u32 alloc_blk() {
    u32 free_blk = first_free();
    SETBIT(free_blk);
    return free_blk;
}

void free_blk(u32 num) {
    CLEARBIT(num);
}

u32 first_free() {
    for (u32 i = 0; i < total_blocks; i++) {
        if (!ISSET(i)) {
            return i;
        }
    }
    printf("pmm: no free blocks");
    return (u32) -1;
}