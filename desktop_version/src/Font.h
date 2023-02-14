/*
 * == SOME TEXT PRINTING EXAMPLES ==
 *
 * Standard print
 *     font::print(0, 50, 50, "Hello world!", 255, 255, 255);
 *
 * Centered text
 *     font::print(PR_CEN, -1, 50, "Hello world!", 255, 255, 255);
 *     (set X to -1, unless you want to center *around* X)
 *
 * 2x scale
 *     font::print(PR_2X, 50, 50, "V", 255, 255, 255);
 *
 * Centered 2x scale
 *     font::print(PR_CEN | PR_2X, -1, 50, "V", 255, 255, 255);
 *
 * Right-aligned 3x scale with a border around it
 *     font::print(PR_RIGHT | PR_3X | PR_BOR, 320, 50, "V", 255, 255, 255);
 *
 * Wordwrapped centered text
 *     font::print_wrap(PR_CEN, -1, 50, "Hello world, this will wordwrap to the screen width", 255, 255, 255);
 *
 */


#ifndef FONT_H
#define FONT_H

#include <SDL.h>
#include <stdint.h>
#include <string>

#include "GraphicsUtil.h"

namespace font
{

#define GLYPH_EXISTS 0x1
#define GLYPH_COLOR 0x2

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

    SDL_Texture* image;

    GlyphInfo* glyph_page[FONT_N_PAGES];
};

struct PrintFlags
{
    uint8_t scale;
    uint8_t font_sel;
    uint8_t alpha;
    uint8_t colorglyph_bri;
    bool border;
    bool align_cen;
    bool align_right;
    bool cjk_low;
    bool cjk_high;
};

#define PR_1X (0 << 0) /* default, 1x scale */
#define PR_2X (1 << 0) /* 2x scale */
#define PR_3X (2 << 0) /* etc */
#define PR_4X (3 << 0)
#define PR_5X (4 << 0)
#define PR_6X (5 << 0)
#define PR_7X (6 << 0)
#define PR_8X (7 << 0)
#define PR_FONT_INTERFACE (0 << 3) /* default, use interface font */
#define PR_FONT_LEVEL (1 << 3) /* use level-specific font (room names, cutscene dialogue, etc) */
#define PR_FONT_8X8 (2 << 3) /* use 8x8 font no matter what */
#define PR_AB_IS_BRI (1 << 16)
#define PR_ALPHA(value) /* use this alpha value 0-255 (incompatible with PR_COLORGLYPH_BRI) */\
    ((~SDL_clamp((int)(value), 0, 255) & 0xff) << 8)
#define PR_COLORGLYPH_BRI(value) /* use this brightness 0-255 for colored glyphs (button icons) */\
    (((~SDL_clamp((int)(value), 0, 255) & 0xff) << 8) | PR_AB_IS_BRI)
#define PR_BOR (1 << 17) /* draw a black border around the text (was bprint/bigbprint) */
#define PR_LEFT (0 << 18) /* default, left-align text/place at x coordinate */
#define PR_CEN (1 << 18) /* center-align text relative to X (X is center) or to screen if X == -1 */
#define PR_RIGHT (2 << 18) /* right-align text to X (X is now the right border, not left border) */
#define PR_CJK_CEN (0 << 20) /* default, larger fonts should stick out on top and bottom compared to 8x8 font */
#define PR_CJK_LOW (1 << 20) /* larger fonts should stick out fully on the bottom (draw at Y) */
#define PR_CJK_HIGH (2 << 20) /* larger fonts should stick out fully on the top */

extern Font temp_bfont;

void load_main(void);
void load_custom(void);
void unload_custom(void);
void destroy(void);

int get_advance(const Font* f, uint32_t codepoint); // TODO de-api

void print(uint32_t flags, int x, int y, const std::string& text, uint8_t r, uint8_t g, uint8_t b);

} // namespace font


#endif // FONT_H
