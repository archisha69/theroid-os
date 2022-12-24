/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: psf.h
*/


//PC Screen Font library
#pragma once

#include <common.h>

#define PSF_FONT_MAGIC 0x864AB672

typedef struct {
    u32 magic;
    u32 version;
    u32 headersize;
    u32 flags;
    u32 numglyph;
    u32 bytesperglyph;
    u32 height;
    u32 width;
} psf_font_t;

void psf_init();