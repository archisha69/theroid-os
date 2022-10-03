/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "math.h"
#include "stdlib.h"
#include "hw/display.h"

extern void *get_ebp();

void memset(void *addr, u8 value, int num) {
  for (int i = 0; i < num; i++) {
    *((u8*)addr + i) = value;
  }
}

void memcpy(void *dest, void *source, int num) {
  for (int i = 0; i < num; i++) {
    *((u8*)(dest + i)) = *((u8*)(source + i));
  }
}

int strcpy(char *dest, char *source) {
    int num = 0;
    while (1) {
        char c = *source;
        *dest = c;
        if (c == '\0') break;
        dest++;
        source++;
        num++;
    }
    return num;
}

u32 strlen(char *str) {
    int i = 0;
    char c;
    while ((c = str[i]) != '\0') i++;
    return i;
}

bool strcmp(char *str1, char *str2) {
    if (strlen(str1) != strlen(str2)) return false;
    for (u32 i = 0; i < strlen(str1); i++) {
        if (str1[i] != str2[i]) return false;
    }
    return true;
}

int int2str(char *dest, int num) {
    u32 start = 0;
    if (num < 0) {
        dest[start++] = '-';
        num = -num;
    }

    char buff[16];
    buff[15] = '\0';
    u32 i = 14;
    while (num > 0) {
        u32 rem = num % 10;
        buff[i--] = '0' + rem;
        num /= 10;
    }

    strcpy(dest + start, buff + i + 1);
    return start + 14 - i;
}

int int2hex(char *dest, int num) {
    u32 start = 0;
    if (num < 0) {
        dest[start++] = '-';
        num = -num;
    }
    dest[start++] = '0';
    dest[start++] = 'x';

    char buff[16];
    buff[15] = '\0';
    u32 i = 14;
    while (num > 0) {
        u32 rem = num % 16;
        if (rem <= 9) {
            buff[i--] = '0' + rem;
        } else {
            buff[i--] = 'a' + (rem - 10);
        }
        num /= 16;
    }

    strcpy(dest + start, buff + i + 1);
    return start + 14 + i;
}

void sprintf_args(char *dest, char *str, void *ptr) {
    int i = 0;
    int write_index = 0;
    while (1) {
        char c = str[i];
        if (c == '\0') break;
        if (c == '%') {
            i++;
            char next = str[i];
            if (c == '\0') break;

            if (next == 'd') {
                int int_arg = *((int *)ptr);
                char buff[16];
                int2str(buff, int_arg);
                write_index += strcpy(dest + write_index, buff);
                ptr += 4;
            } else if (next == 'u') {
                u32 int_arg = *((u32*)ptr);
                char buff[16];
                int2str(buff, int_arg);
                write_index += strcpy(dest + write_index, buff);
                ptr += 4;
            } else if (next == 'x') {
                int int_arg = *((int*)ptr);
                char buff[16];
                int2hex(buff, int_arg);
                write_index += strcpy(dest + write_index, buff);
            } else if (next == 's') {
                char *str_arg = *((char**)ptr);
                write_index += strcpy(dest + write_index, str_arg);
                ptr += 4;
            }
        } else {
            dest[write_index++] = c;
        }
        i++;
    }
    dest[write_index] = '\0';
}

void sprintf(char *dest, char *str, ...) {
    void *ebp = get_ebp();
    void *ptr = ebp + 16;
    sprintf_args(dest, str, ptr);
}
