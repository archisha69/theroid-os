/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: speakers.c
*/

#include <io.h>

static void play_sound(u32 freq) {
    u32 div;
    u8 tmp;

    div = 1193180 / freq;
    outb(PIT_CMD, 0xB6);
    outb(PIT_DAT2, (u8) div);
    outb(PIT_DAT2, (u8) (div >> 8));

    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

static void stop_sound() {
    u8 tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

void beep(u32 ms) {
    play_sound(1000);
    //sleep(ms);
    //stop_sound();
}