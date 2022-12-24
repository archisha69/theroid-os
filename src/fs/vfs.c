/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: vfs.c
*/

#include <fs/vfs.h>

fs_node_t *fs_root = 0;

u32 read_fs(fs_node_t *node, u32 off, u32 size, u8 *buff) {
    if (node->read != 0) {
        return node->read(node, off, size, buff);
    }
    return 0;
}

u32 write_fs(fs_node_t *node, u32 off, u32 size, u8 *buff) {
    if (node->write != 0) {
        return node->write(node, off, size, buff);
    }
    return 0;
}

void close_fs(fs_node_t *node) {
    if (node->close != 0) {
        return node->close(node);
    }
}

void open_fs(fs_node_t *node, u8 read, u8 write) {
    if (node->open != 0) {
        return node->open(node);
    }
}

struct dirent *readdir_fs(fs_node_t *node, u32 index) {
    if ((node->flags & 0x7) == FS_DIR && node->readdir != 0) {
        return node->readdir(node, index);
    }
    return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    if ((node->flags & 0x7) == FS_DIR && node->readdir != 0) {
        return node->finddir(node, name);
    }
}