/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "isr.h"
#include "ports.h"

#include "../kernel.h"
#include "../drivers/display.h"

u32 tick = 0;

static void timer_callback(registers_t *regs) {
    tick++;
}

u32 gettick() {
    return tick;
}

void init_timer(u32 freq) {
    if (freq == 0) panic("Invalid timer frequency");
    register_interrupt_handler(IRQ0, timer_callback);

    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
