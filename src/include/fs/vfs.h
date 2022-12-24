/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: vfs.h
*/

#pragma once

#include <common.h>

#define FS_FILE       0x01
#define FS_DIR        0x02
#define FS_CHARDEV    0x03
#define FS_BLKDEV     0x04
#define FS_PIPE       0x05
#define FS_SYMLINK    0x06
#define FS_MOUNTPOINT 0x08

typedef u32 (*read_type_t) (struct fs_node*, u32, u32, u8*);
typedef u32 (*write_type_t) (struct fs_node*, u32, u32, u8*);
typedef void (*open_type_t) (struct fs_node*);
typedef void (*close_type_t) (struct fs_node*);
typedef struct dirent* (*readdir_type_t) (struct fs_node*, u32);
typedef struct fs_node* (*finddir_type_t) (struct fs_node*, char *name);

typedef struct fs_node {
    char name[128];
    u32 mask;
    u32 uid;
    u32 gid;
    u32 flags;
    u32 inode;
    u32 len;
    u32 impl;

    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr;
} fs_node_t;

struct dirent {
    char name[128];
    u32 ino;
};

extern fs_node_t *fs_root;

u32 read_fs(fs_node_t *node, u32 off, u32 size, u8 *buff);
u32 write_fs(fs_node_t *node, u32 off, u32 size, u8 *buff);
void open_fs(fs_node_t *node, u8 read, u8 write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, u32 index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);