/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

void memory_copy(u8* source, u8* dest, u32 nbytes);
void init_dynamic_mem();
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void* p);