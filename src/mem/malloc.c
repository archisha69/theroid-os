/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: malloc.c
*/

#include <malloc.h>

static u8 mem_area[DYNAMIC_MEM_SIZE];
static mem_node_t *mem_start;

static void *find_best_blk(mem_node_t *mem, u32 size) {
    mem_node_t *best_blk = (mem_node_t *) nullptr;
    u32 blk_size = DYNAMIC_MEM_SIZE + 1;
    mem_node_t *crt_blk = mem;

    while (crt_blk) {
        if ((!crt_blk->used) &&
            (crt_blk->size >= (size + MEM_NODE_SIZE)) &&
            (crt_blk->size <= blk_size)) {
                best_blk = crt_blk;
                blk_size = crt_blk->size;
        }
        crt_blk = crt_blk->next;
    }

    return best_blk;
}

static void *merge_next_to_crt(mem_node_t *crt_node) {
    mem_node_t *next_mem_node = crt_node->next;
    if (next_mem_node != nullptr && !next_mem_node->used) {
        crt_node->size += crt_node->next->size;
        crt_node->size += MEM_NODE_SIZE;
        crt_node->next = crt_node->next->next;

        if (crt_node->next != nullptr) {
            crt_node->next->prev = crt_node;
        }
    }
    return crt_node;
}

static void *merge_crt_to_prev(mem_node_t *crt_node) {
    mem_node_t *prev_node = crt_node->prev;
    if (prev_node != nullptr && !prev_node->used) {
        prev_node->size += crt_node->size;
        prev_node->size += MEM_NODE_SIZE;

        prev_node->next = crt_node->next;
        if (crt_node->next != nullptr) {
            crt_node->next->prev = prev_node;
        }
    }
    return crt_node;
}

void init_dynamic_mem() {
    mem_start = (mem_node_t *) mem_area;
    mem_start->size = DYNAMIC_MEM_SIZE - MEM_NODE_SIZE;
    mem_start->next = nullptr;
    mem_start->prev = nullptr;
}

void *malloc(u32 size) {
    mem_node_t *best_blk = (mem_node_t *) find_best_blk(mem_start, size);

    if (best_blk != nullptr) {
        best_blk->size = best_blk->size - size - MEM_NODE_SIZE;
        mem_node_t *alloc = (mem_node_t *) (((u8 *) best_blk) + MEM_NODE_SIZE + best_blk->size);
        alloc->size = size;
        alloc->used = true;
        alloc->next = best_blk->next;
        alloc->prev = best_blk;

        if (best_blk->next != nullptr) {
            best_blk->next->prev = alloc;
        }
        best_blk->next = alloc;
        return (void *) ((u8 *) alloc + MEM_NODE_SIZE);
    }

    return nullptr;
}

void mfree(void *p) {
    if (p == nullptr) {
        return;
    }

    mem_node_t *crt_node = (mem_node_t *) ((u8 *) p - DYNAMIC_MEM_SIZE);
    if (crt_node == nullptr) {
        return;
    }

    crt_node->used = false;
    crt_node = merge_next_to_crt(crt_node);
    merge_crt_to_prev(crt_node);
}

void bzero(void *s, u32 n) {
    char *c = s;
    for (int i = 0; i < n; i++) {
        c[i] = '\0';
    }
}

void *calloc(u32 nmemb, u32 size) {
    if (nmemb == 0 || size == 0) {
        return nullptr;
    }

    char *p = malloc(nmemb * size);
    bzero(p, nmemb * size);
    return p;
}