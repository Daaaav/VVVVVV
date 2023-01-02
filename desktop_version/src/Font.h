#ifndef FONT_H
#define FONT_H

#include <SDL.h>
#include <stdint.h>

#include "GraphicsUtil.h"

namespace font
{

struct GlyphInfo
{
    uint16_t image_idx;
    uint8_t advance;
    uint8_t flags;
};

/* Codepoints go up to U+10FFFF, so we have 0x110 (272) pages
 * of 0x1000 (4096) glyphs, allocated as needed */
#define FONT_N_PAGES 0x110
#define FONT_PAGE_SIZE 0x1000

struct Font
{
    uint8_t glyph_w;
    uint8_t glyph_h;

    uint16_t n_x_glyphs;
    uint16_t n_y_glyphs;

    SDL_Surface* image;

    SDL_Surface* scratch_1x;
    SDL_Surface* scratch_8x;

    GlyphInfo* glyph_page[FONT_N_PAGES];
};

extern Font temp_bfont;

void load_main(void);
void load_custom(void);
void unload_custom(void);
void destroy(void);

int get_advance(const Font* f, uint32_t codepoint); // TODO de-api
int print_char(const Font* f, SDL_Surface* dest_surface, uint32_t codepoint, int x, int y, int scale, SDL_Color color); // TODO de-api

} /* namespace font */


#endif /* FONT_H */
