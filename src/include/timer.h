/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: timer.h
*/

#pragma once

#include <common.h>

void init_timer(u32 freq);
u32 get_ticks();
void sleep(double sec);