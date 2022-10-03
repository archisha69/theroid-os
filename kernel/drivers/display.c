/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "mem.h"
#include "types.h"
#include "ports.h"
#include "stdlib.h"
#include "hw/display.h"

#define TAB_LENGTH 4

void set_cursor(int offset) {
    offset /= 2;
    outb(REG_SCREEN_CTRL, 14);
    outb(REG_SCREEN_DATA, (u8) (offset >> 8));
    outb(REG_SCREEN_CTRL, 15);
    outb(REG_SCREEN_DATA, (u8) (offset & 0xff));
}

int get_cursor() {
    outb(REG_SCREEN_CTRL, 14);
    int offset = inb(REG_SCREEN_DATA) << 8;
    outb(REG_SCREEN_CTRL, 15);
    offset += inb(REG_SCREEN_DATA);
    return offset * 2;
}

static inline int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

static inline int get_row_from_offset(int offset) {
    return offset / (2 * MAX_COLS);
}

static inline int move_offset_to_new_line(int offset) {
    return get_offset(0, get_row_from_offset(offset) + 1);
}

static inline void set_char_at_video_memory(char character, int offset) {
    volatile u8 *vidmem = (u8*) VIDEO_ADDRESS;
    vidmem[offset] = character;
    vidmem[offset + 1] = WHITE_ON_BLACK;
}

static int scroll_ln(int offset) {
    memcpy(
            (u8*)(get_offset(0, 1) + VIDEO_ADDRESS),
            (u8*)(get_offset(0, 0) + VIDEO_ADDRESS),
            MAX_COLS * (MAX_ROWS - 1) * 2
    );

    for (int col = 0; col < MAX_COLS; col++) {
        set_char_at_video_memory(' ', get_offset(col, MAX_ROWS - 1));
    }

    return offset - 2 * MAX_COLS;
}

static inline void print_nl() {
    int newOffset = move_offset_to_new_line(get_cursor());
    if (newOffset >= MAX_ROWS * MAX_COLS * 2) {
        newOffset = scroll_ln(newOffset);
    }
    set_cursor(newOffset);
}

static inline void print_backspace() {
    int newCursor = get_cursor() - 2;
    set_char_at_video_memory(' ', newCursor);
    set_cursor(newCursor);
}

void print_char(char c) {
    int offset = get_cursor();
    if (offset >= MAX_ROWS * MAX_COLS * 2) {
        offset = scroll_ln(offset);
    }
    if (c == '\n') {
        print_nl();
    } else if (c == '\t') {
        for (char i = 0; i < TAB_LENGTH; i++) {
            print_char(' ');
        }
    } else if (c == '\b') {
        print_backspace();
    } else {
        set_char_at_video_memory(c, offset);
        offset += 2;
        set_cursor(offset);
    }
}

void print_string(char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        print_char(string[i]);
    }
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    for (int i = 0; i < screen_size; ++i) {
        set_char_at_video_memory(' ', i * 2);
    }
    set_cursor(get_offset(0, 0));
}