/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: math.c
*/

#include <math.h>

static u32 next = 1;
static const int a = 1103515245;
static const int b = 69420;  //this number can be anything
static const int m = 0x80000000; //max value

u32 max(u32 a, u32 b) {
    return a > b ? a : b;
}

u32 min(u32 a, u32 b) {
    return a < b ? a : b;
}

u32 pow(u32 a, u32 b) {
    u32 res = a;
    for (int i = 1; i < b; i++) {
        res *= a;
    }
    return res;
}

u32 abs(int n) {
    if (n < 0) {
        return -n;
    }
    return n;
}

void srand(u32 seed) {
    next = seed;
}

u32 rand() {
    if (next == 0) {
        srand(1);
        srand(rand());
    }
    next = (a * next + b) % m; //https://en.wikipedia.org/wiki/Pseudorandom_number_generator
    return next;
}