/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: timer.c
*/

#include <io.h>
#include <isr.h>
#include <rtc.h>
#include <timer.h>
#include <stdio.h>
#include <task/task.h>

extern rtc_time_t crt_time;

double tick = 0;

static void timer_callback(regs_t regs) {
    tick++;
    task_switch();
}

void sleep(double sec) {
    double start = tick;
    while (tick < start + sec) {
        asm("hlt");
        task_switch();
    }
}

void init_timer(u32 freq) {
    register_interrupt_handler(IRQ0, timer_callback);

    u32 div = 1193180 / freq;
    outb(PIT_CMD, 0x36);

    u8 low = div & 0xFF;
    u8 high = (div & 0xFF) >> 8;

    outb(PIT_DAT0, low);
    outb(PIT_DAT0, high);
}

u32 get_ticks() {
    return tick;
}