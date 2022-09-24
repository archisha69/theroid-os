/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "mem.h"
#include "idt.h"
#include "isr.h"
#include "rand.h"
#include "stdio.h"
#include "stdlib.h"
#include "ports.h"
#include "types.h"
#include "string.h"
#include "timer.h"
#include "kernel.h"

#include "hal/hal.h"
#include "../drivers/drive.h"
#include "../drivers/display.h"
#include "../drivers/keyboard.h"

void start_kernel() {
    clear_screen();

    #ifdef DEBUG
    printf("Initializing interrupt service routines\n");
    #endif
    isr_install();

    #ifdef DEBUG
    printf("Enabling external interrupts\n");
    #endif
    asm volatile("sti");

    #ifdef DEBUG
    printf("Initializing timer (IRQ 0)\n");
    #endif
    init_timer(50);

    #ifdef DEBUG
    printf("[%u] Initializing dynamic memory\n", gettick());
    #endif
    init_dynamic_mem();

    #ifdef DEBUG
    printf("[%u] Testing dynamic memory\n", gettick());
    void *ptr = malloc(10);
    if (ptr == nullptr) panic("malloc: failed to allocate memory");
    ptr = realloc(ptr, 20);
    if (ptr == nullptr) panic("realloc: failed to reallocate memory");
    void *ptr1 = calloc(2, 10);
    if (ptr1 == nullptr) panic("calloc: failed to allocate memory");
    free(ptr);
    free(ptr1);
    #endif

    #ifdef DEBUG
    printf("[%u] Initializing keyboard (IRQ 1)\n", gettick());
    #endif
    init_keyboard();

    #ifdef DEBUG
    printf("[%u] Initializing hard drive (IRQ 2, 3)\n", gettick());
    #endif
    init_drive();

    printf("Welcome to theroid os!\ntype help for a command list\n>> ");
}

void execute_command(char *input) {
    printf("\n");
    input = trim(input);
    if (strcmp(input, "halt") || strcmp(input, "shutdown")) {
        shutdown();
    } else if (strcmp(input, "clear")) {
        clear_screen();
        printf(">> ");
    } else if (strcmp(input, "help")) {
        printf(
            "halt - stops the system\n"
            "clear - clears the screen\n"
            "echo <text> - prints text\n"
            "panic - triggers a kenrel panic\n"
            "reboot - reboots the system by causing a triple cpu fault\n"
            ">> "
        );
    } else if (startswith(input, "echo")) {
        char **arr = split(input, ' ');
        int i = 1;
        while (!strcmp(arr[i], "\0")) {
            printf(arr[i]);
            i++;
        }
        printf("\n>> ");
    } else if (startswith(input, "panic")) {
        panic("e");
    } else if (strcmp(input, "reboot")) {
        reboot();
    } else if(strcmp(input, "rand")) {
        //srand(gettick());
        printf(rand_string(rand() % 32, STRING_LETTERS));
    }
    else {
        printf("Unknown command: %s\n>> ", input);
    }
}

void panic(char *message) {
    clear_screen();
    printf("----beginning of kernel panic----\n\n[%u] %s\n\n-------end of kernel panic-------\n", gettick(), message);
    asm volatile("cli\nhlt");
}