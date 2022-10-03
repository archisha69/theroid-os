/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "mem.h"
#include "assert.h"
#include "stdio.h"
#include "types.h"
#include "stdlib.h"
#include "string.h"

void append(char *s, char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

bool startswith(char str[], char what[]) {
    int len = strlen(what);
    for (int i = 0; i < len; i++) {
        if (what[i] != str[i]) {
            return false;
        }
    }
    return true;
}

char *strtok(char *s, char *delm) {
    int q = 0;

    if (!s || !delm || s[q] == '\0') return nullptr;

    char *c = (char*) malloc(sizeof(char) * 100);
    int i = q;
    int j = 0;
    int k = 0;

    while (s[i] != '\0') {
        j = 0;
        while (delm[j] != '\0') {
            if (s[i] != delm[j]) {
                c[k] = s[i];
            } else {
                c[i] = 0;
                q = i + 1;
                return c;
            }
            j++;
        }
        i++;
        k++;
    }

    //should never reach here
    return nullptr;
}

char *strdup(char *token) {
    char *str;
    char *ptr;
    int len = 0;

    while (token[len]) {
        len++;
    }
    str = malloc(len + 1);
    ptr = str;
    while (*token) {
        *ptr = *token++;
    }
    *ptr = '\0';

    return str;
}

char *strcat(char *dest, char *src) {
    char *ptr = dest + strlen(dest);

    while (*src != '\0') {
        *ptr++ = *src;
    }
    *ptr = '\0';
    return dest;
}

int split(char *str, char delimiter, char ***dest) {
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0') {
        if (*p == delimiter) count++;
        p++;
    }

    *dest = (char **) malloc(count);
    if (*dest == 0) return 1;
    p = str;
    while (*p != '\0') {
        if (*p == delimiter) {
            (*dest)[i] = (char *) malloc(token_len);
            if ((*dest)[i] == 0) return 1;
            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*dest)[i] = (char *) malloc(token_len);
    if ((*dest)[i] == 0) return 1;
    i = 0;
    p = str;
    t = ((*dest)[i]);
    while (*p != '\0') {
        if (*p != delimiter && *p != '\0') {
            *t = *p;
            t++;
        } else {
            *t = '\0';
            i++;
            t = ((*dest)[i]);
        }
        p++;
    }

    return count;
}

char *trim(char *str) {
    char *end;
    while (*str == ' ') str++;
    if (*str == 0) return str;
    end = str + strlen(str)-1;
    while (end > str && *end == ' ') end--;
    end[1] = '\0';
    return str;
}

bool isdigit(char c) {
    if (c >= '0' && c <= '9') return true;
    return false;
}