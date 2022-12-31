/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: kernel.c
*/

#include <io.h>
#include <vga.h>
#include <gdt.h>
#include <idt.h>
#include <psf.h>
#include <pmm.h>
#include <rtc.h>
#include <timer.h>
#include <kheap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <paging.h>
#include <multiboot.h>

#include <hal/panic.h>
#include <task/task.h>
#include <fs/initrd.h>
#include <hw/speaker.h>
#include <hw/keyboard.h>

extern u32 placement_addr;

u32 initial_esp;

void panic(char *fmt, ...) {
    char *str;

    va_list arg;
    va_start(arg, fmt);
    vsprintf(str, fmt, arg);
    va_end(arg);

    clear();
    printf("-----------kernel panic----------\n%s\n-------end of kernel panic-------", str);
    disable_cursor();

    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}

void reboot() {
    asm volatile("cli");

    u32 temp;
    while ((temp & 0x02) != 0) {
        temp = inb(0x64);
        if ((temp & 0x01) != 0) {
            inb(0x60);
        }
    }
    outb(0x64, 0xFE);
    while (1);
}

int system(char *cmd) {
    printf("\n");
    cmd = trim(cmd);
    char **argv;    
    int argc = split(cmd, ' ', &argv);
    int status = ERR_CMD_NOTFOUND;

    if (strcmp(argv[0], "clear")) {
        clear();
        printf(">> ");
        status = EXIT_SUCCESS;
    } else if (strcmp(argv[0], "reboot")) {
        reboot();
        status =  EXIT_SUCCESS;
    }else if (strcmp(argv[0], "echo")) {
        if (argc == 1) {
            print("echo: repeats your text\n\tUsage: echo <text>\n>> ");
            status = ERR_NO_ARGS;
        }

        for (int i = 1; i < argc; i++) {
            print(argv[i]);
            printc(' ');
        }

        print("\n>> ");
        status = EXIT_SUCCESS;
    } else if (strcmp(argv[0], "rand")) {
        if (argc == 1) {
            printf("%d", rand());
        } else {
            int max = atoi(argv[1]);
            printf("%d", rand() % max);
        }
        print("\n>> ");
        status = EXIT_SUCCESS;
    } else if (strcmp(argv[0], "panic")) {
        panic("manually initiated panic");
        status = EXIT_SUCCESS;
    } else if (strcmp(argv[0], "help")) {
        print("Command list:\n\tclear - clears the screen\n\treboot - reboots the system\n\techo <text> - repeats your text\n\trand - returns a random number\n\tls - shows the contents of the root directory\n>> ");
        status = EXIT_SUCCESS;
    } else if (strcmp(argv[0], "ls")) {
        int i = 0;
        struct dirent *node = 0;
        print("Contents of root directory:\n");
        while ((node = readdir_fs(fs_root, i)) != 0) {
            printf("\t%s", node->name);
            
            fs_node_t *fsnode = finddir_fs(fs_root, node->name);
            if ((fsnode->flags & 0x7) == FS_DIR) {
                print("\t\tDIR\n");
            } else {
                if (argc > 1 && strcmp(argv[1], "-c") /*show contents*/) {
                    print("\n\t\tcontents: ");
                    u8 buff[256];
                    u32 size = read_fs(fsnode, 0, 256, buff);

                    for (int j = 0; j < size; j++) {
                        printc(buff[j]);
                    }
                } else {
                    printc('\n');
                }
            }
            i++;
        }
        print("\n>> ");
        status = EXIT_SUCCESS;
    } else if (strcmp(argv[0], "time")) {
        print_crt_time();
        print("\n>> ");
        status = EXIT_SUCCESS;
    }
    
    if (status == ERR_CMD_NOTFOUND) {
        printf("Unknown command: %s\n>> ", argv[0]);
    }

    for (int i = 0; i < argc; i++) {
        kfree(argv[i]);
    }

    return status;
}

void start_kernel(struct multiboot *mboot_ptr, u32 initial_stack) {
    asm volatile("cli");
    initial_esp = initial_stack;
    clear();

    print("Initializing global descriptor table\n");
    init_gdt();

    print("Initializing interrupt descriptor table\n");
    init_idt();

    print("Initializing interrupt mask register\n");
    outb(PIC_MASTER_DAT, 0xF9);

    print("Initializing PC screen font\n");
    psf_init();

    print("Setting initrd addresses\n");
    ASSERT(mboot_ptr->mods_count > 0);
    u32 initrd_pos = *((u32 *) mboot_ptr->mods_addr);
    u32 initrd_end = *(u32 *) (mboot_ptr->mods_addr + 4);
    placement_addr = initrd_end;

    print("Initializing paging\n");
    init_paging();  

    print("Initializing tasking system\n");
    init_tasking();

    print("Initializing PIT (IRQ 0)\n");
    init_timer(100);

    print("Initializing keyboard (IRQ 1)\n");
    init_keyboard();

    print("Setting up initrd root\n");
    fs_root = init_initrd(initrd_pos);

    print("Initializing physical memory manager\n");
    init_pmm(1073741824); // 1 GB

    print("\nInitializing Real Time Clock (IRQ 8)\n");
    init_rtc();
    print_crt_time();

    print("\nEnabling interrupts\n");
    asm volatile("sti");
    //clear();

    print("Copyright (c) thatOneArchUser 2022-2023\n\nWelcome to reflect os!\nType 'help' for a command list\n>> ");
    return;
}