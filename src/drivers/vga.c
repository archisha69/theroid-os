/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: vga.h
*/

#include <io.h>
#include <vga.h>
#include <stdlib.h>

#define TAB_LENGTH 4

u16 *vidmem = (u16 *) VIDEO_ADDR;

int cur_y, cur_x = 0;

static void move_cursor() {
    u32 off = get_offset();
    outb(VGA_CMD, 14);
    outb(VGA_DAT, off >> 8);
    outb(VGA_CMD, 15);
    outb(VGA_DAT, off);
}

static void scroll() {
    if (cur_y >= VGA_HEIGHT) {
        int i;
        for (i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vidmem[i] = vidmem[VGA_WIDTH + i];
        }

        for (i = (VGA_HEIGHT - 1 ) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vidmem[i] = 0x20 | (WHITE_ON_BLACK << 8);
        }
        cur_y = VGA_HEIGHT - 1;
    }
}

static void print_backspace() {
    vidmem[get_offset() - 1] = 0x20 | (WHITE_ON_BLACK << 8);
    cur_x--;
    move_cursor();
}

int get_offset() {
    return cur_y * VGA_WIDTH + cur_x;
}

int get_x() {
    return cur_x;
}

void disable_cursor() {
    outb(VGA_CMD, 0x0A);
    outb(VGA_DAT, 0x20);
}

void printc(char c) {
    u16 *pos;

    if (c == '\b' && cur_x) {
        print_backspace();
    } else if (c == '\t') {
        for (int i = 0; i < TAB_LENGTH; i ++) {
            printc(0x20);
        }
    } else if (c == '\n') {
        cur_x = 0;
        cur_y++;
    } else {
        pos = vidmem + (get_offset());
        *pos = c | (WHITE_ON_BLACK << 8);
        cur_x++;
    }

    if (cur_x >= VGA_WIDTH) {
        cur_x = 0;
        cur_y++;
    }
    scroll();
    move_cursor();
}

void clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vidmem[i] = 0x20 | (WHITE_ON_BLACK << 8);
    }
    cur_x = 0;
    cur_y = 0;
    move_cursor();
}

void print(char *string) {
    int i = 0;
    while (string[i]) {
        printc(string[i++]);
    }
}