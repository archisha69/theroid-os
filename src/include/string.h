/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: string.h
*/

#pragma once

#include <common.h>

void append(char *s, char n);
void ftoa(char *buff, double f);
void *memset(void *dest, u8 c, int n);
void memcpy(void *dest, void *src, int num);
int strcpy(char *dest, char *src);
bool strcmp(char *a, char *b);
u32 strlen(char *s);
u16 *memsetw(u16 *dest, u16 val, u32 count);
int itoa(char *dest, int num);
int int2hex(char *dest, int num);
int memcmp(void *a, void *b, u32 n);
bool isdigit(char c);
bool isnumber(char *c);
char *strcat(char *dest, char *src);
char *strdup(char *token);
char *strtok(char *s, char *delm);
bool startswith(char str[], char c[]);
int split(char *str, char delimiter, char ***dest);
char *trim(char *str);
int atoi(char *str);
