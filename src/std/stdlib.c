/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: stdlib.c
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void vsprintf(char *dest, char *fmt, va_list list) {
    int i = 0;
    int idx = 0;

    while (1) {
        char c = fmt[i];
        if (c == '\0') {
            break;
        }

        if (c == '%') {
            i++;
            char next = fmt[i];
            if (c == '\0') {
                break;
            }

            switch (next) {
                case 'd': {
                    int int_arg = va_arg(list, int);
                    char buff[16];
                    itoa(buff, int_arg);
                    idx += strcpy(dest + idx, buff);
                    break;
                }
                case 'u': {
                    u32 int_arg = va_arg(list, u32);
                    char buff[16];
                    itoa(buff, int_arg);
                    idx += strcpy(dest + idx, buff);
                    break;
                }
                case 'x': {
                    int int_arg = va_arg(list, int);
                    char buff[16];
                    int2hex(buff, int_arg);
                    idx += strcpy(dest + idx, buff);
                    break;
                }
                case 'c': {
                    char char_arg = va_arg(list, int);
                    char buff[] = {char_arg, '\0'};
                    idx += strcpy(dest + idx, buff);
                    break;
                }
                case 's': {
                    char *str_arg = va_arg(list, char*);
                    idx += strcpy(dest + idx, str_arg);
                    break;
                }
                case 'f': {
                    double double_arg = va_arg(list, double);
                    char buff[10];
                    ftoa(buff, double_arg);
                    idx += strcpy(dest + idx, buff);
                    break;
                }
            }
        } else {
            dest[idx++] = c;
        }
        i++;
    }

    dest[idx] = '\0';
}

void sprintf(char *dest, char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    vsprintf(dest, fmt, arg);
    va_end(arg);
}