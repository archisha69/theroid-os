/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#define DEBUG
#define INT_STOP asm volatile("cli")
#define INT_START asm volatile("sti")
#define HALT asm volatile("hlt")

void panic(char *message);