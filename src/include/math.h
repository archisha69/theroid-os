/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: math.h
*/

#pragma once

#include <common.h>

typedef struct {
    double Re;
    double Im;
} complex_t;

u32 max(u32 a, u32 b);
u32 min(u32 a, u32 b);
u32 pow(u32 a, u32 b);
u32 abs(int n);

void srand(u32 seed);
u32 rand();