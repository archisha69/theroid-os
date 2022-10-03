/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "types.h"
#include "hw/display.h"

static void _printdec(int n) {
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


static void _printhex(int n) {
    char res[12];
    int i = 0;
    while (n != 0) {
        int temp = 0;
        temp = n % 16;

        if (temp < 10) {
            res[i] = temp + 48;
            i++;
        }
        else {
            res[i] = temp + 55;
            i++;
        }
        n = n / 16;
    }
    int zeros = 8 - strlen(res);
    for (int k = 0; k < zeros; k++) print_char('0');
    for (int j = i - 1; j >= 0; j--) print_char(res[j]);
}

static void _vprintf(char *fmt, va_list list) {
    int i = 0;

    while (1) {
        char c = fmt[i];
        if (c == '\0') break;

        if (c == '%') {
            i++;
            char next = fmt[i];
            if (c == '\0') break;

            if (next == 'd') {
                int int_arg = va_arg(list, int);
                _printdec(int_arg);
            } else if (next == 'u') {
                u32 int_arg = va_arg(list, u32);
                _printdec(int_arg);
            } else if (next == 'x') {
                u32 int_arg = va_arg(list, u32);
                print_string("0x");
                _printhex(int_arg);
            } else if (next == 'c') {
                char char_arg = va_arg(list, char);
                print_char(char_arg);
            } else if (next == 's') {
                char *str_arg = va_arg(list, char*);
                print_string(str_arg);
            } else if (next == 'f') {
                float float_arg = va_arg(list, double);
                long s = (long) float_arg;
                if (s == float_arg) {
                    _printdec(s);
                } else {
                    char *buff;
                    unsigned long decimalpart = (float_arg - s) * pow(10, 9);
                    int2str(buff, decimalpart);
                    for (int i = strlen(buff); i > 0; i--) {
                        if (buff[i] == '0') buff[i] = '\0';
                    }
                    printf("%d.%s", s, buff);
                }
            } else if (next == 'b') {
                bool bool_arg = va_arg(list, int);
                print_string((bool_arg) ? "true" : "false");
            }
        } else {
            print_char(c);
        }
        i++;
    }
}

void printf(char *str, ...) {
    va_list arg;
    va_start(arg, str);
    _vprintf(str, arg);
    va_end(arg);
}
