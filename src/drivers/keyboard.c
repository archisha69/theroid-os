#include "hw/keyboard.h"
#include <io.h>
#include <isr.h>
#include <vga.h>
#include <string.h>
#include <stdlib.h>

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

static char key_buffer[256];

#define ENTER 0x1C
#define BACKSPACE 0x0E

static bool backspace(char s[]) {
    int len = strlen(s);
    if (len > 0) {
        s[len - 1] = '\0';
        return true;
    } else {
        return false;
    }
}

static void keyboard_callback() {
    u8 status;
    u16 scancode;
    status = inb(KEYBOARD_STATUS);
    if (status & 0x01) {
        scancode = inb(KEYBOARD_DAT);
        if (scancode == BACKSPACE) {
            if (get_x() > 3) {
                printc('\b');
                backspace(key_buffer);
            }
        }
        if (scancode == ENTER) {
            if (strlen(key_buffer) == 0) {
                printc('\n');
                print(">> ");
            } else {
                system(key_buffer);
                while (backspace(key_buffer));
            }
        }
        if (scancode <= 57 && sc_ascii[scancode] != '?') {
            if (strlen(key_buffer) == 256) {
                return;
            }
            char letter = sc_ascii[scancode];
            append(key_buffer, letter);
            char str[2] = {letter, '\0'};
            print(str);
            inb(KEYBOARD_STATUS);
        }
    }
    outb(0x20, 0x20);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}