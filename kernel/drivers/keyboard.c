/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#include "isr.h"
#include "types.h"
#include "ports.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "hal/hal.h"

#include "display.h"
#include "keyboard.h"
#include "../kernel.h"

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

static char key_buffer[256];
static bool shift;
static bool alt;
static bool ctrl;

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

static void keyboard_callback(registers_t *regs) {
    u8 status;
    u16 scancode;
    u16 i = 0;
    char release;
    status = port_byte_in(0x64);
    if (status & 0x01) {
        scancode = port_byte_in(0x60);
        if (ctrl && scancode == SYSRQ) { 
            if (strlen(key_buffer) == 0) panic("next time dont spam random keys (ctrl + alt + sysrq)");
            else panic(key_buffer);
        }
        if (ctrl && alt && scancode == DELETE) reboot();
        if (scancode == ALT) alt = true;
        if (scancode == ALT_RELEASE) alt = false;
        if (scancode == CTRL) ctrl = true;
        if (scancode == CTRL_RELEASE) ctrl = false;
        if (scancode == HOME) {
            while (backspace(key_buffer)) set_cursor(get_cursor() - 1);
        }
        if (scancode == SHIFT || scancode == RSHIFT) shift = true;
        if (scancode == SHIFT_RELEASE || scancode == RSHIFT_RELEASE) shift = false;
        if (scancode == BACKSPACE) {
            if (backspace(key_buffer)) print_char('\b');
        }
        if (scancode == LEFT_ARROW) {
            if (backspace(key_buffer)) set_cursor(get_cursor()-1);
        }
        if (scancode == UP_ARROW) {
            if (!(i >= (sizeof(history) / sizeof(history[0])))) {
                while (backspace(key_buffer)) print_char('\b');
                printf(history[i]);
                strcpy(key_buffer, history[i]);
                i++;
            }
        }
        if (scancode == DOWN_ARROW) {
            if (i > 0) {
                while (backspace(key_buffer)) print_char('\b');
                i--;
                printf(history[i]);
                strcpy(key_buffer, history[i]);
            } else if (i == 0) {
                while (backspace(key_buffer)) print_char('\b');
            }
        }
        if (scancode == CAPS_LOCK) shift = !shift;
        if (scancode == ENTER) {
            if (strlen(key_buffer) == 0) {
                print_char('\n');
                print_string(">> ");
            } else {
                if (strcmp(key_buffer, "reboot")) {
                    key_buffer[0] = '\0';
                    reboot(); //buffer isnt cleared before reboot
                }
                execute_command(key_buffer);
                key_buffer[0] = '\0';
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
            release = port_byte_in(0x60);
        }
    }
    port_byte_out(0x20, 0x20);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}
