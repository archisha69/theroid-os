/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "isr.h"
#include "math.h"
#include "mem.h"
#include "stdlib.h"

#include "drive.h"

static void drive_interrupt_handler() {}

void init_drive() {
    register_interrupt_handler(IRQ2, drive_interrupt_handler);
    register_interrupt_handler(IRQ3, drive_interrupt_handler);
}

extern void read_disk(char *buff, u32 start_sector, u32 sector_num);

static void read_sector(char *buff, u32 sector) {
    read_disk(buff, sector, 1);
}

void read_drive(char *buff,  u32 start, u32 len) {
    u32 end = start + len;
    u32 start_sector = start / SECTOR_SIZE;
    u32 end_sector = (end - 1) / SECTOR_SIZE + 1;
    char *sector_buff = (char*) malloc(SECTOR_SIZE);

    for (u32 i = start_sector; i < end_sector; i++) {
        read_sector(sector_buff, i);
        u32 copy_start_addr = max(i * SECTOR_SIZE, start);
        u32 copy_end_addr = min((i + 1) * SECTOR_SIZE, end);
        u32 copy_size = copy_end_addr - copy_start_addr;
        memcpy(buff, sector_buff + copy_start_addr - i * SECTOR_SIZE, copy_size);
        buff += copy_size;
    }
    free(sector_buff);
}