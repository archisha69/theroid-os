/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: ordered_array.h
*/

#pragma once

#include <common.h>

typedef void *type_t;
typedef char (*predicate_t)(type_t, type_t);

typedef struct {
    type_t *arr;
    u32 size;
    u32 max;
    predicate_t less_than;
} ordered_array_t;

char std_lt_predicate(type_t a, type_t b); //standard less than predicate

ordered_array_t new_oarr(u32 max, predicate_t p);
ordered_array_t place_oarr(void *addr, u32 max, predicate_t p);

void destroy_oarr(ordered_array_t *arr);
void ins_oarr(type_t item, ordered_array_t *arr);
type_t lookup_oarr(u32 i, ordered_array_t *arr);
void rm_oarr(u32 i, ordered_array_t *arr);