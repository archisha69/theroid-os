/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: initrd.h
*/

#include <fs/vfs.h>

typedef struct {
    u32 nfiles;
} initrd_header_t;

typedef struct {
    u8 magic;
    u8 name[64];
    u32 off;
    u32 len;
} initrd_file_t;

fs_node_t *init_initrd(u32 loc);