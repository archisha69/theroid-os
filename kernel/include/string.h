/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)
#define STRING_LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define STRING_UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define STRING_LETTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define STRING_DIGITS "0123456789"

void append(char *s, char n);
bool startswith(char str[], char what[]);
char *strtok(char *s, char *delm);
char *strdup(char *token);
char *strcat(char *dest, char *src);
int split(char *str, char delimiter, char ***dest);
char *trim(char *str);
bool isdigit(char c);