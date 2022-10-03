/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "isr.h"
#include "ports.h"

#include "kernel.h"
#include "hw/display.h"

u32 tick = 0;

static void timer_callback() {
    tick++;
}

void sleep(u32 ms) {
    u32 t = tick + ms;
    while (tick < t);
}

u32 gettick() {
    return tick;
}

void init_timer(u32 freq) {
    if (freq == 0) panic("Invalid timer frequency");
    register_interrupt_handler(IRQ0, timer_callback);

    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)((divisor >> 8) & 0xFF);
    outb(0x43, 0x36);
    outb(0x40, low);
    outb(0x40, high);
}
