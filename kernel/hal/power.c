/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "ports.h"
#include "kernel.h"

void reboot() {
    INT_STOP;
    unsigned temp;
    do {
        temp = inb(0x64);
        if ((temp & 0x01) != 0) {
            inb(0x60);
            continue;
        } 
    } while ((temp & 0x02) != 0);
    outb(0x64, 0xFE);
    while (1);
}

void shutdown() {
    for (char *q = "Shutdown"; *q; ++q) outb(0x8900, *q);
    outb(0xf4, 0x00); //if above function fails
}