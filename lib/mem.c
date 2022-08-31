/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserverd
*/

#include "mem.h"
#include "../drivers/display.h"
#include "../lib/string.h"

void memory_copy(u8 *source, u8 *dest, u32 nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

#define DYNAMIC_MEM_TOTAL_SIZE 4*1024
#define DYNAMIC_MEM_NODE_SIZE sizeof(dynamic_mem_node_t)

typedef struct dynamic_mem_node {
    uint32_t size;
    bool used;
    struct dynamic_mem_node *next;
    struct dynamic_mem_node *prev;
} dynamic_mem_node_t;

static uint8_t dynamic_mem_area[DYNAMIC_MEM_TOTAL_SIZE];
static dynamic_mem_node_t *dynamic_mem_start;

void init_dynamic_mem() {
    dynamic_mem_start = (dynamic_mem_node_t *) dynamic_mem_area;
    dynamic_mem_start->size = DYNAMIC_MEM_TOTAL_SIZE - DYNAMIC_MEM_NODE_SIZE;
    dynamic_mem_start->next = NULL_POINTER;
    dynamic_mem_start->prev = NULL_POINTER;
}

void print_dynamic_node_size() {
    char node_size_string[256];
    int_to_string(DYNAMIC_MEM_NODE_SIZE, node_size_string);
    print_string("DYNAMIC_MEM_NODE_SIZE = ");
    print_string(node_size_string);
    print_nl();
}

void print_dynamic_mem_node(dynamic_mem_node_t *node) {
    char size_string[256];
    int_to_string(node->size, size_string);
    print_string("{size = ");
    print_string(size_string);
    char used_string[256];
    int_to_string(node->used, used_string);
    print_string("; used = ");
    print_string(used_string);
    print_string("}; ");
}

void print_dynamic_mem() {
    dynamic_mem_node_t *current = dynamic_mem_start;
    print_string("[");
    while (current != NULL_POINTER) {
        print_dynamic_mem_node(current);
        current = current->next;
    }
    print_string("]\n");
}

void *find_best_mem_block(dynamic_mem_node_t* dynamic_mem, size_t size) {
    dynamic_mem_node_t *best_mem_block = (dynamic_mem_node_t*) NULL_POINTER;
    uint32_t best_mem_block_size = DYNAMIC_MEM_TOTAL_SIZE + 1;
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

void *mem_alloc(size_t size) {
    dynamic_mem_node_t *best_mem_block = (dynamic_mem_node_t*) find_best_mem_block(dynamic_mem_start, size);

    if (best_mem_block != NULL_POINTER) {
        best_mem_block->size = best_mem_block->size - size - DYNAMIC_MEM_NODE_SIZE;
        dynamic_mem_node_t *mem_node_allocate = (dynamic_mem_node_t*) (((uint8_t*) best_mem_block) + DYNAMIC_MEM_NODE_SIZE + best_mem_block->size);
        mem_node_allocate->size = size;
        mem_node_allocate->used = true;
        mem_node_allocate->next = best_mem_block->next;
        mem_node_allocate->prev = best_mem_block;

        if (best_mem_block->next != NULL_POINTER) {
            best_mem_block->next->prev = mem_node_allocate;
        }
        best_mem_block->next = mem_node_allocate;
        return (void *) ((uint8_t *) mem_node_allocate + DYNAMIC_MEM_NODE_SIZE);
    }

    return NULL_POINTER;
}

void *merge_next_node_into_current(dynamic_mem_node_t* current_mem_node) {
    dynamic_mem_node_t *next_mem_node = current_mem_node->next;
    if (next_mem_node != NULL_POINTER && !next_mem_node->used) {
        current_mem_node->size += current_mem_node->next->size;
        current_mem_node->size += DYNAMIC_MEM_NODE_SIZE;
        current_mem_node->next = current_mem_node->next->next;
        if (current_mem_node->next != NULL_POINTER) {
            current_mem_node->next->prev = current_mem_node;
        }
    }
    return current_mem_node;
}

void *merge_current_node_into_previous(dynamic_mem_node_t* current_mem_node) {
    dynamic_mem_node_t *prev_mem_node = current_mem_node->prev;
    if (prev_mem_node != NULL_POINTER && !prev_mem_node->used) {
        prev_mem_node->size += current_mem_node->size;
        prev_mem_node->size += DYNAMIC_MEM_NODE_SIZE;

        prev_mem_node->next = current_mem_node->next;
        if (current_mem_node->next != NULL_POINTER) {
            current_mem_node->next->prev = prev_mem_node;
        }
    }
    return current_mem_node;
}

void mem_free(void *p) {
    if (p == NULL_POINTER) {
        return;
    }
    dynamic_mem_node_t *current_mem_node = (dynamic_mem_node_t *) ((uint8_t *) p - DYNAMIC_MEM_NODE_SIZE);
    if (current_mem_node == NULL_POINTER) {
        return;
    }
    current_mem_node->used = false;
    current_mem_node = merge_next_node_into_current(current_mem_node);
    merge_current_node_into_previous(current_mem_node);
}

void* alloc(TYPE_t t, int n) {
    void *ptr;
    switch (t) {
        case INT: {
            ptr = (int *) mem_alloc(n * sizeof(int));
            break;
        }
        case CHAR: {
            ptr = (char *) mem_alloc(n * sizeof(char));
            break;
        }
        case VOID: {
            ptr = (void *) mem_alloc(n * sizeof(void*));
        }
        case PCHAR: {
            ptr = (char **) mem_alloc(n * sizeof(char*));
        }
        default: {
            ptr = (void *) mem_alloc(n * sizeof(void*));
        }
    }
    if (ptr == NULL_POINTER) {
        print_string("Memory not allocated\n");
    }
    return ptr;
}

void test_dynamic_mem(){
    int *ptr1 = alloc(INT, 5);
    print_string("int *ptr1 = alloc(5)\n");
    print_dynamic_mem();
    print_nl();

    int *ptr2 = alloc(INT, 10);
    print_string("int *ptr2 = alloc(10)\n");
    print_dynamic_mem();
    print_nl();

    mem_free(ptr1);
    print_string("mem_free(ptr1)\n");
    print_dynamic_mem();
    print_nl();

    int *ptr3 = alloc(INT, 2);
    print_string("int *ptr3 = alloc(2)\n");
    print_dynamic_mem();
    print_nl();

    mem_free(ptr2);
    print_string("mem_free(ptr2)\n");
    print_dynamic_mem();
    print_nl();

    mem_free(ptr3);
    print_string("mem_free(ptr3)\n");
    print_dynamic_mem();
    print_nl();
}