/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "mem.h"
#include "string.h"
#include "types.h"
#include "stdio.h"
#include "stdlib.h"
#include "kernel.h"

#define DYNAMIC_MEM_TOTAL_SIZE 4*1024
#define DYNAMIC_MEM_NODE_SIZE sizeof(dynamic_mem_node_t)

typedef struct dynamic_mem_node {
    u32 size;
    bool used;
    struct dynamic_mem_node *next;
    struct dynamic_mem_node *prev;
} dynamic_mem_node_t;

static u8 dynamic_mem_area[DYNAMIC_MEM_TOTAL_SIZE];
static dynamic_mem_node_t *dynamic_mem_start;

static void bzero(void *s, size_t n) {
    char *c = s;
    size_t i;
    for (i = 0; i < n; i++) {
        c[i] = '\0';
    }
}

void init_dynamic_mem() {
    dynamic_mem_start = (dynamic_mem_node_t *) dynamic_mem_area;
    dynamic_mem_start->size = DYNAMIC_MEM_TOTAL_SIZE - DYNAMIC_MEM_NODE_SIZE;
    dynamic_mem_start->next = nullptr;
    dynamic_mem_start->prev = nullptr;
}

static void *find_best_mem_block(dynamic_mem_node_t* dynamic_mem, size_t size) {
    dynamic_mem_node_t *best_mem_block = (dynamic_mem_node_t*) nullptr;
    u32 best_mem_block_size = DYNAMIC_MEM_TOTAL_SIZE + 1;
    dynamic_mem_node_t *current_mem_block = dynamic_mem;
    while (current_mem_block) {
        if ((!current_mem_block->used) &&
            (current_mem_block->size >= (size + DYNAMIC_MEM_NODE_SIZE)) &&
            (current_mem_block->size <= best_mem_block_size)) {
            best_mem_block = current_mem_block;
            best_mem_block_size = current_mem_block->size;
        }
        current_mem_block = current_mem_block->next;
    }
    return best_mem_block;
}

void *malloc(size_t size) {
    dynamic_mem_node_t *best_mem_block = (dynamic_mem_node_t*) find_best_mem_block(dynamic_mem_start, size);

    if (best_mem_block != nullptr) {
        best_mem_block->size = best_mem_block->size - size - DYNAMIC_MEM_NODE_SIZE;
        dynamic_mem_node_t *mem_node_allocate = (dynamic_mem_node_t*) (((u8*) best_mem_block) + DYNAMIC_MEM_NODE_SIZE + best_mem_block->size);
        mem_node_allocate->size = size;
        mem_node_allocate->used = true;
        mem_node_allocate->next = best_mem_block->next;
        mem_node_allocate->prev = best_mem_block;

        if (best_mem_block->next != nullptr) {
            best_mem_block->next->prev = mem_node_allocate;
        }
        best_mem_block->next = mem_node_allocate;
        return (void *) ((u8 *) mem_node_allocate + DYNAMIC_MEM_NODE_SIZE);
    }
    
    #ifdef DEBUG
    printf("malloc: memory not allocated\n");
    #endif
    return nullptr;
}

void *calloc(size_t nmemb, size_t size) {
    char *ptr;
    if (nmemb == 0 || size == 0) {
        return nullptr;
    } else {
        ptr = malloc(nmemb * size);
        bzero(ptr, nmemb * size);
        return ptr;
    }
}

void *realloc(void *ptr, size_t size) {
    void *res;
    if (ptr == nullptr) {
        res = malloc(size);
    } else {
        if (sizeof(ptr) < size) {
            res = malloc(size);
            if (res == nullptr) return nullptr;

            memcpy(res, ptr, sizeof(ptr));
            free(ptr);
        } else {
            res = ptr;
        }
    }
    return res;
}

static void *merge_next_node_into_current(dynamic_mem_node_t* current_mem_node) {
    dynamic_mem_node_t *next_mem_node = current_mem_node->next;
    if (next_mem_node != nullptr && !next_mem_node->used) {
        current_mem_node->size += current_mem_node->next->size;
        current_mem_node->size += DYNAMIC_MEM_NODE_SIZE;
        current_mem_node->next = current_mem_node->next->next;
        if (current_mem_node->next != nullptr) {
            current_mem_node->next->prev = current_mem_node;
        }
    }
    return current_mem_node;
}

static void *merge_current_node_into_previous(dynamic_mem_node_t* current_mem_node) {
    dynamic_mem_node_t *prev_mem_node = current_mem_node->prev;
    if (prev_mem_node != nullptr && !prev_mem_node->used) {
        prev_mem_node->size += current_mem_node->size;
        prev_mem_node->size += DYNAMIC_MEM_NODE_SIZE;

        prev_mem_node->next = current_mem_node->next;
        if (current_mem_node->next != nullptr) {
            current_mem_node->next->prev = prev_mem_node;
        }
    }
    return current_mem_node;
}

void free(void *p) {
    if (p == nullptr) {
        return;
    }
    dynamic_mem_node_t *current_mem_node = (dynamic_mem_node_t *) ((u8 *) p - DYNAMIC_MEM_NODE_SIZE);
    if (current_mem_node == nullptr) {
        return;
    }
    current_mem_node->used = false;
    current_mem_node = merge_next_node_into_current(current_mem_node);
    merge_current_node_into_previous(current_mem_node);
}