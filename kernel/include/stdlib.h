/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

void memset(void *addr, u8 val, int num);
void memcpy(void *addr, void *source, int num);
i32 strcpy(char *dest, char *source);
bool strcmp(char *str1, char *str2);
u32 strlen(char *str);
i32 int2str(char *dest, i32 num);
i32 int2hex(char *dest, i32 num);
void sprintf_args(char *dest, char *str, void *ptr);
void sprintf(char *dest, char *str, ...);