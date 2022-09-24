/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "types.h"
#include "stdio.h"
#include "stdlib.h"
#include "../drivers/display.h"

extern void *get_ebp();

static void _printdec(i32 n) {
    if (n == 0) print_string("0");
    if (n < 0) {
        print_string("-");
        n = -n;
    }

    u32 acc = n;
    char c[32];
    int i = 0;
    while (acc > 0) {
        c[i] = '0' + acc % 10;
        acc /= 10;
        i++;
    }
    c[i] = 0;
    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while (i >= 0) {
        c2[i--] = c[j++];
    }
    print_string(c2);
}

static void _printhex(u32 n) {
    i32 tmp;
    print_string("0x");

    bool bl;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && bl) continue;

        if (tmp >= 0xA) {
            bl = false;
            print_char(tmp - 0xA + 'a');
        } else {
            bl = false;
            print_char(tmp + '0');
        }
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) {
        print_char(tmp - 0xA + 'a');
    } else {
        print_char(tmp + '0');
    }
}

void printf(char* str, ...) {
    void *ebp = get_ebp();
    void *ptr = ebp + 12;
    int i = 0;

    while (1) {
        char c = str[i];
        if (c == '\0') break;

        if (c == '%') {
            i++;
            char next = str[i];
            if (c == '\0') break;

            if (next == 'd') {
                i32 int_arg = *((i32*) ptr);
                _printdec(int_arg);
                ptr += 4;
            } else if (next == 'u') {
                u32 int_arg = *((u32*) ptr);
                _printdec(int_arg);
                ptr += 4;
            } else if (next == 'x') {
                u32 int_arg = *((u32*) ptr);
                _printhex(int_arg);
                ptr += 4;
            } else if (next == 'c') {
                char char_arg = *((char*) ptr);
                print_char(char_arg);
                ptr += 4;
            } else if (next == 's') {
                char *str_arg = *((char**) ptr);
                print_string(str_arg);
                ptr += 4;
            }
        } else {
            print_char(c);
        }
        i++;
    }
}