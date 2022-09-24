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

bool backspace(char s[]) {
    int len = strlen(s);
    if (len > 0) {
        s[len - 1] = '\0';
        return true;
    } else {
        return false;
    }
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

char **split(char *str, char delm) {
    char **res;
    size_t count;
    char *tmp;
    char *last;
    char delim[2];
    delim[0] = delm;
    delim[1] = 0;

    while (*tmp) {
        if (delm == *tmp) {
            count++;
            last = tmp;
        }
        tmp++;
    }

    count += last < (str + strlen(str) - 1);
    count++;

    res = malloc(sizeof(char*) * count);
    if (res) {
        size_t idx = 0;
        char *token = strtok(str, delim);

        while (token) {
            ASSERT(idx < count);
            *(res + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        ASSERT(idx == count - 1);
        *(res + idx) = 0;
    }

    return res;
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