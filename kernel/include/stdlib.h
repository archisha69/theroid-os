/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

void memset(void *addr, u8 val, int num);
void memcpy(void *addr, void *source, int num);
int strcpy(char *dest, char *source);
bool strcmp(char *str1, char *str2);
u32 strlen(char *str);
int int2str(char *dest, int num);
int int2hex(char *dest, int num);
void sprintf_args(char *dest, char *str, void *ptr);
void sprintf(char *dest, char *str, ...);
