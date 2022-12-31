/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: io.c
*/

#include <common.h>

void outb(u16 port, u8 val) {
    asm volatile("outb %1, %0" : : "dN" (port), "a" (val));
}

void outw(u16 port, u16 val) {
    asm volatile("outw %1, %0" : : "dN" (port), "a" (val));
}

u8 inb(u16 port) {
    u8 ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

u16 inw(u16 port) {
    u16 ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void io_wait() {
    asm volatile("outb %%al, $0x80" :: "a"(0));
}