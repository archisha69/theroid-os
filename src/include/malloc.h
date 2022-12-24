/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: malloc.h
*/

#pragma once

#include <common.h>

#define DYNAMIC_MEM_SIZE 4096
#define MEM_NODE_SIZE sizeof(mem_node_t)

typedef struct mem_node {
    u32 size;
    bool used;
    struct mem_node *next;
    struct mem_node *prev;
} mem_node_t;

void init_dynamic_mem();
void *malloc(u32 size);
void mfree(void *p);
void *calloc(u32 nmemb, u32 size);
void bzero(void *s, u32 n);