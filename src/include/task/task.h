/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: task.h
*/

#pragma once

#include <paging.h>

#define KERNEL_STACK_SIZE 2048

typedef struct task {
    int id;
    u32 kernel_stack;
    regs_t regs;
    pagedir_t *dir;
    struct task *next;
} task_t;

void init_tasking();
void task_switch();
int fork();
int getpid();
void move_stack(void *new_stack_start, u32 size);
void switch_to_usermode();