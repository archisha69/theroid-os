/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "isr.h"
#include "rand.h"
#include "timer.h"
#include "types.h"
#include "ports.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "kernel.h"

#include "hal/hal.h"
#include "hw/display.h"

#define HOME 0x47
#define END 0x4F

#define SYSRQ 0x54
#define SYSRQ_RELEASE 0xD4
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define DELETE 0x53
#define ALT 0x38
#define ALT_RELEASE 0xB8
#define CTRL 0x1D
#define CTRL_RELEASE 0x9D
#define TAB 0x0F

#define SHIFT 0x2A
#define RSHIFT 0x36
#define CAPS_LOCK 0x3A
#define SHIFT_RELEASE 0xAA 
#define RSHIFT_RELEASE 0xB6

#define UP_ARROW 0x48
#define DOWN_ARROW 0x50
#define RIGHT_ARROW 0x4D
#define LEFT_ARROW 0x4B

#define FUNCTION1 0x3B
#define FUNCTION2 0x3C
#define FUNCTION3 0x3D
#define FUNCTION4 0x3E
#define FUNCTION5 0x3F
#define FUNCTION6 0x40
#define FUNCTION7 0x41
#define FUNCTION8 0x42
#define FUNCTION9 0x43
#define FUNCTION10 0x44

static char *key_buffer;
static bool shift;
static bool alt;
static bool ctrl;
static bool q = false;

const char *sc_name[] = {"ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                         "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                         "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                         "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                         "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                         "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii_shift[] = {'?', '?', '!', '@', '#', '$', '%', '^',
                         '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
                         'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G',
                         'H', 'J', 'K', 'L', ':', '\"', '~', '/', '|', 'Z', 'X', 'C', 'V',
                         'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '};
const char sc_ascii[] = {'?', '?', '1', '2', '3', '4', '5', '6',
                         '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
                         'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
                         'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
                         'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

static bool backspace(char s[]) {
    int len = strlen(s);
    if (len > 0) {
        s[len - 1] = '\0';
        return true;
    } else {
        return false;
    }
}

static void execute_command(char *input) {
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
        char **arr;
        int tokens = split(input, ' ', &arr);
        for (int i = 1; i < tokens; i++) {
            printf(arr[i]);
            print_char(' ');
        }
        printf("\n>> ");
    } else if (startswith(input, "panic")) {
        panic("e");
    } else if (strcmp(input, "reboot")) {
        reboot();
    } else if(strcmp(input, "rand")) {
        srand(gettick());
        printf("%s\n>> ", rand_string(rand() % 32, STRING_LETTERS));
    }
    else {
        printf("Unknown command: ");
        printf(input);
        printf("\n>> ");
    }
}

static void keyboard_callback() {
    if (!q)  {
        append(key_buffer, 'e');
        while (backspace(key_buffer)); //key_buffer[0] = '\0'; causes invalid opcode exception
        q = true;
    }
    u8 status;
    u16 scancode;
    status = inb(0x64);
    if (status & 0x01) {
        scancode = inb(0x60);
        if (ctrl && scancode == SYSRQ) { 
            if (strlen(key_buffer) == 0) panic("next time dont spam random keys (ctrl + alt + sysrq)");
            else panic(key_buffer);
        }
        if (ctrl && alt && scancode == DELETE) reboot();
        if (scancode == ALT) alt = true;
        if (scancode == ALT_RELEASE) alt = false;
        if (scancode == CTRL) ctrl = true;
        if (scancode == CTRL_RELEASE) ctrl = false;
        if (scancode == SHIFT || scancode == RSHIFT) shift = true;
        if (scancode == SHIFT_RELEASE || scancode == RSHIFT_RELEASE) shift = false;
        if (scancode == BACKSPACE) {
            if (backspace(key_buffer)) print_char('\b');
        }
        if (scancode == CAPS_LOCK) shift = !shift;
        if (scancode == ENTER) {
            if (strlen(key_buffer) == 0) {
                print_char('\n');
                print_string(">> ");
            } else {
                if (strcmp(key_buffer, "reboot")) {
                    while (backspace(key_buffer));  //buffer isnt cleared before reboot
                    reboot();
                }
                execute_command(key_buffer);
                while (backspace(key_buffer));
            }
        }
        if (scancode == TAB) {
            for (int i = 0; i < 4; i++) append(key_buffer, ' ');
            print_string("    ");
        }
        if (scancode == 0x47 && shift == true) {
            append(key_buffer, '?');
            print_string("?");
        }
        if (scancode <= 57 && sc_ascii[scancode] != '?') {
            if (strlen(key_buffer) == 256) return;
            char letter;
            if (shift) letter = sc_ascii_shift[(int) scancode];
            else letter = sc_ascii[(int) scancode];
            append(key_buffer, letter);
            char str[2] = {letter, '\0'};
            print_string(str);
            inb(0x60);
        }
    }
    outb(0x20, 0x20);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}