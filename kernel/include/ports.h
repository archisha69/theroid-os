/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

inline u8 inb(u16 port) {
    u8 result;
    asm volatile("inb %w1, %b0" : "=a" (result) : "Nd" (port));
    return result;
}

inline u16 inw(u16 port) {
    u16 result;
    asm volatile("inw %w1, %w0" : "=a" (result) : "Nd" (port));
    return result;
}

inline u32 inl(u16 port) {
    u32 result;
    asm volatile("inl %w1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

inline void outb(u16 port, u8 data) {
    asm volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

inline void outw(u16 port, u16 data) {
    asm volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

inline void outl(u16 port, u32 data) {
    asm volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

inline void insl(u8 port, u32 *buff, int quads) {
    for (int i = 0; i < quads; i++) {
        buff[i] = inl(port);
    }
}