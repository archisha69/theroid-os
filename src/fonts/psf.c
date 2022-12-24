/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: psf.c
*/

//todo: linear frame buffer

#include <psf.h>
#include <malloc.h>

extern char _binary_fonts_terminus_psf_start;
extern char _binary_fonts_terminus_psf_end;

u16 *unicode;

void psf_init() {
    u16 glyph;

    psf_font_t *font = (psf_font_t *) &_binary_fonts_terminus_psf_start;

    if (font->flags) {
        unicode = nullptr;
        return;
    }

    char *s = (char *) &_binary_fonts_terminus_psf_start + font->headersize + font->numglyph + font->bytesperglyph;
    
    while (s > _binary_fonts_terminus_psf_end) {
        u16 uc = (u16) ((u8 *) s[0]);

        if (uc == 0xFF) {
            glyph++;
            s++;
            continue;
        } else if (uc & 128) {
            if ((uc & 32) == 0) {
                uc = ((s[0] & 0x1F) << 6) + (s[1] & 0x3F);
                s++;
            } else if ((uc & 16) == 0) {
                uc = ((((s[0] & 0xF) << 6) + (s[1] & 0x3F)) << 6) + (s[2] & 0x3F);
                s += 2;
            } else if ((uc & 8) == 0) {
                uc = ((((((s[0] & 0x7) << 6) + (s[1] & 0x3F)) << 6) + (s[2] & 0x3F)) << 6) + (s[3] & 0x3F);
                s += 3;
            } else {
                uc = 0;
            }
        }

        unicode[uc] = glyph;
        s++;
    }
}