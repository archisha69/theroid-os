/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: panic.h
*/

#pragma once

#define ASSERT(b) ((b) ? (void) 0 : panic("%s, %d, %s", __FILE__, __LINE__, #b))

void panic(char *fmt, ...);