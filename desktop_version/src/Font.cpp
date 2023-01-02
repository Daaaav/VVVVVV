#include "Font.h"

#include <tinyxml2.h>
#include <utf8/unchecked.h>

#include "Alloc.h"
#include "FileSystemUtils.h"
#include "Graphics.h"

// Sigh... This is the second forward-declaration, we need to put this in a header file
SDL_Surface* LoadImage(const char *filename);

namespace font
{

Font temp_bfont; // replace with like, a vector of all loaded fonts

static void codepoint_split(
    const uint32_t codepoint,
    short* page,
    short* glyph
)
{
    /* Splits a code point (0x10FFFF) into page (0x10F) and glyph (0xFFF) */
    if (codepoint > 0x10FFFF)
    {
        codepoint_split(0xFFFD, page, glyph);
        return;
    }
    *page = codepoint >> 12;
    *glyph = codepoint % FONT_PAGE_SIZE;
}

static GlyphInfo* get_glyphinfo(
    const Font* f,
    const uint32_t codepoint
)
{
    short page, glyph;
    codepoint_split(codepoint, &page, &glyph);

    if (f->glyph_page[page] == NULL)
    {
        return NULL;
    }

    return &f->glyph_page[page][glyph];
}

static GlyphInfo* add_glyphinfo(
    Font* f,
    const uint32_t codepoint
)
{
    short page, glyph;
    codepoint_split(codepoint, &page, &glyph);

    if (f->glyph_page[page] == NULL)
    {
        f->glyph_page[page] = (GlyphInfo*) SDL_calloc(FONT_PAGE_SIZE, sizeof(GlyphInfo));
        if (f->glyph_page[page] == NULL)
        {
            return NULL;
        }
    }

    return &f->glyph_page[page][glyph];
}

static bool glyph_is_valid(const GlyphInfo* glyph)
{
    return glyph->flags == 1; // TODO flags
}

static GlyphInfo* find_glyphinfo(const Font* f, const uint32_t codepoint)
{
    /* TODO document */
    GlyphInfo* glyph = get_glyphinfo(f, codepoint);
    if (glyph == NULL || !glyph_is_valid(glyph))
    {
        glyph = get_glyphinfo(f, 0xFFFD);
        if (glyph == NULL || !glyph_is_valid(glyph))
        {
            glyph = get_glyphinfo(f, '?');
            if (glyph == NULL || !glyph_is_valid(glyph))
            {
                return NULL;
            }
        }
    }

    return glyph;
}

static void load_font(Font* f, const char* name)
{
    char name_png[256];
    char name_txt[256];
    char name_xml[256];
    SDL_snprintf(name_png, sizeof(name_png), "graphics/%s.png", name);
    SDL_snprintf(name_txt, sizeof(name_txt), "graphics/%s.txt", name);
    SDL_snprintf(name_xml, sizeof(name_xml), "graphics/%s.fontinfo", name);

    f->glyph_w = 8;
    f->glyph_h = 8;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    bool xml_loaded = false;

    if (FILESYSTEM_loadAssetTiXml2Document(name_xml, doc))
    {
        xml_loaded = true;

        //f->glyph_w =
        //f->glyph_h =
    }

    f->image = LoadImage(name_png);
    f->scratch_1x = RecreateSurfaceWithDimensions(f->image, f->glyph_w, f->glyph_h);
    f->scratch_8x = RecreateSurfaceWithDimensions(f->image, f->glyph_w*8, f->glyph_h*8);
    SDL_zeroa(f->glyph_page);

    if (f->image == NULL || f->scratch_1x == NULL || f->scratch_8x == NULL)
    {
        return;
    }

    f->n_x_glyphs = f->image->w / f->glyph_w;
    f->n_y_glyphs = f->image->h / f->glyph_h;

    unsigned char* charmap = NULL;
    size_t length;
    if (FILESYSTEM_areAssetsInSameRealDir(name_png, name_txt))
    {
        FILESYSTEM_loadAssetToMemory(name_txt, &charmap, &length, false);
    }
    if (charmap == NULL)
    {
        /* If we don't have font.txt, it's 2.2-style plain ASCII */
        length = 0x80;
        charmap = (unsigned char*) SDL_malloc(length);
        if (charmap == NULL)
        {
            return;
        }
        for (uint8_t codepoint = 0x00; codepoint < length; codepoint++)
        {
            charmap[codepoint] = codepoint;
        }
    }
    unsigned char* current = charmap;
    unsigned char* end = charmap + length;
    int pos = 0;
    while (current != end)
    {
        int codepoint = utf8::unchecked::next(current);
        GlyphInfo* glyph = add_glyphinfo(f, codepoint);
        if (glyph == NULL)
        {
            break;
        }
        glyph->image_idx = pos;
        glyph->advance = f->glyph_w;
        glyph->flags = 1; // TODO flags
        ++pos;
    }

    VVV_free(charmap);

    // TODO get data from font.xml
    for (int codepoint = 0; codepoint < 32; codepoint++)
    {
        GlyphInfo* glyph = get_glyphinfo(f, codepoint);
        if (glyph != NULL)
        {
            glyph->advance = 6;
        }
    }
}

void load_main(void)
{
    // TODO PHYSFS_enumerateFiles, load everything that matches *.fontinfo or font.png (but not font.fontinfo)
    load_font(&temp_bfont, "font");
}

void load_custom(void)
{
    // Custom (level-specific assets) fonts NYI
}

void unload_custom(void)
{
    /* Unload all custom fonts */

}

void destroy(void)
{
    /* Unload all fonts (main and custom) for exiting */
    Font* f = &temp_bfont;
    VVV_freefunc(SDL_FreeSurface, f->image);

    for (int i = 0; i < FONT_N_PAGES; i++)
    {
        VVV_free(f->glyph_page[i]);
    }
}

static inline void image_idx_to_xy(const Font* f, const uint16_t image_idx, int* x, int* y)
{
    *x = (image_idx % f->n_x_glyphs) * f->glyph_w;
    *y = (image_idx / f->n_x_glyphs) * f->glyph_h;
}

int get_advance(const Font* f, const uint32_t codepoint)
{
    /* Get the width of a single character in a font */
    GlyphInfo* glyph = find_glyphinfo(f, codepoint);;
    if (glyph == NULL)
    {
        return f->glyph_w;
    }

    return glyph->advance;
}

int print_char(
    const Font* f,
    SDL_Surface* dest_surface,
    const uint32_t codepoint,
    const int x,
    const int y,
    const int scale,
    const SDL_Color color
)
{
    /* Draws the glyph for a codepoint at x,y on dest_surface.
     * Returns the amount of pixels to advance the cursor. */
    GlyphInfo* glyph = find_glyphinfo(f, codepoint);;
    if (glyph == NULL)
    {
        return f->glyph_w * scale;
    }

    int src_x, src_y;
    image_idx_to_xy(f, glyph->image_idx, &src_x, &src_y);
    SDL_Rect src_rect = {src_x, src_y, f->glyph_w, f->glyph_h};

    int draw_w = f->glyph_w*scale;
    int draw_h = f->glyph_h*scale;
    SDL_Surface* src_surface;

    if (scale > 1 || graphics.flipmode)
    {
        /* Now we need to scale and/or flip the character... */
        ClearSurface(f->scratch_1x);
        SDL_BlitSurface(f->image, &src_rect, f->scratch_1x, NULL);
        src_rect.x = 0;
        src_rect.y = 0;
        if (graphics.flipmode)
        {
            FlipSurfaceVertical(f->scratch_1x, NULL);
        }
        if (scale > 1)
        {
            src_rect.w = draw_w;
            src_rect.h = draw_h;
            ClearSurface(f->scratch_8x);
            SDL_BlitScaled(f->scratch_1x, NULL, f->scratch_8x, &src_rect);
            src_surface = f->scratch_8x;
        }
        else
        {
            src_surface = f->scratch_1x;
        }
    }
    else
    {
        src_surface = f->image;
    }

    SDL_Rect dest_rect = {x, y, draw_w, draw_h};
    BlitSurfaceColoured(src_surface, &src_rect, dest_surface, &dest_rect, color);

    return glyph->advance * scale;
}

} /* namespace font */
