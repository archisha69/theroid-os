/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: vga.h
*/

#pragma once

#include <common.h>

#define VIDEO_ADDR 0xB8000
#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define WHITE_ON_BLACK 0x0F

void disable_cursor();
void set_cursor(int off);
void printc(char c);
void print(char *string);
void clear();
int get_offset();
int get_x();