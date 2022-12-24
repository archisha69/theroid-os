/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: timer.c
*/

#include <io.h>
#include <isr.h>
#include <timer.h>

u32 tick = 0;

static void timer_callback(regs_t regs) {
    tick++;
}

void init_timer(u32 freq) {
    register_interrupt_handler(IRQ0, timer_callback);

    u32 div = 1193180 / freq;
    outb(0x43, 0x36);

    u8 low = div & 0xFF;
    u8 high = (div >> 8) & 0xFF;

    outb(0x40, low);
    outb(0x40, high);
}