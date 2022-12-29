/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: pmm.h
*/

#include <common.h>

#define BLOCK_SIZE        4096
#define BLOCKS_PER_BUCKET 8

#define SETBIT(a)        bitmap[a / BLOCKS_PER_BUCKET] = bitmap[a / BLOCKS_PER_BUCKET] | (1 << (a % BLOCKS_PER_BUCKET))
#define CLEARBIT(a)      bitmap[a / BLOCKS_PER_BUCKET] = bitmap[a / BLOCKS_PER_BUCKET] & (~(1 << (a % BLOCKS_PER_BUCKET)))
#define ISSET(a)         ((bitmap[i / BLOCKS_PER_BUCKET] >> (a % BLOCKS_PER_BUCKET)) & 0x1)
#define GET_BUCKET32(a)  (*((u32 *) &bitmap[a / 32]))

#define BLK_ALIGN(a) (((a) & 0xFFFFF000) + 0x1000)

void init_pmm(u32 sz);
u32 alloc_blk();
void free_blk(u32 num);
u32 first_free();