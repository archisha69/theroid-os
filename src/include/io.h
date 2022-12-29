/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: io.h
*/

#include <common.h>

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DAT 0x21
#define PIC_SLAVE_CMD  0xA0
#define PIC_SLAVE_DAT  0xA1

#define PIC_EIO 0x20

#define VGA_CMD 0x3D4
#define VGA_DAT 0x3D5

#define KEYBOARD_DAT 0x60
#define KEYBOARD_STATUS 0x64

#define PIT_DAT0 0x40
#define PIT_DAT1 0x41
#define PIT_DAT2 0x42
#define PIT_CMD  0x43

void outb(u16 port, u8 val);
u8 inb(u16 port);