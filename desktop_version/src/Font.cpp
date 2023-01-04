#include "Font.h"

#include <tinyxml2.h>
#include <utf8/unchecked.h>

#include "Alloc.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
#include "UtilityClass.h"
#include "XMLUtils.h"

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
    return glyph->flags & GLYPH_EXISTS;
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
    SDL_snprintf(name_xml, sizeof(name_xml), "graphics/%s.fontmeta", name);

    f->glyph_w = 8;
    f->glyph_h = 8;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (FILESYSTEM_loadAssetTiXml2Document(name_xml, doc))
    {
        hDoc = hDoc.FirstChildElement("font_metadata");

        pElem = hDoc.FirstChildElement("width").ToElement();
        if (pElem != NULL)
        {
            f->glyph_w = help.Int(pElem->GetText());
        }
        pElem = hDoc.FirstChildElement("height").ToElement();
        if (pElem != NULL)
        {
            f->glyph_h = help.Int(pElem->GetText());
        }
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
        if (glyph == NULL && glyph_is_valid(glyph))
        {
            break;
        }
        glyph->image_idx = pos;
        glyph->advance = f->glyph_w;
        glyph->flags = GLYPH_EXISTS;
        ++pos;
    }

    VVV_free(charmap);

    pElem = hDoc.FirstChildElement("special").ToElement();
    if (pElem != NULL)
    {
        tinyxml2::XMLElement* subElem;
        FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
        {
            EXPECT_ELEM(subElem, "range");

            unsigned start, end;
            if (subElem->QueryUnsignedAttribute("start", &start) != tinyxml2::XML_SUCCESS
                || subElem->QueryUnsignedAttribute("end", &end) != tinyxml2::XML_SUCCESS
                || end < start || start > 0x10FFFF)
            {
                continue;
            }
            end = SDL_min(end, 0x10FFFF);

            int advance = subElem->IntAttribute("advance", -1);
            int color = subElem->IntAttribute("color", -1);

            for (uint32_t codepoint = start; codepoint <= end; codepoint++)
            {
                GlyphInfo* glyph = get_glyphinfo(f, codepoint);
                if (glyph != NULL)
                {
                    if (advance >= 0 && advance < 256)
                    {
                        glyph->advance = advance;
                    }
                    if (color == 0)
                    {
                        glyph->flags &= ~GLYPH_COLOR;
                    }
                    else if (color == 1)
                    {
                        glyph->flags |= GLYPH_COLOR;
                    }
                }
            }
        }
    }
}

void load_main(void)
{
    // TODO PHYSFS_enumerateFiles, load everything that matches *.fontmeta or font.png (but not font.fontmeta)
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
    if (glyph->flags & GLYPH_COLOR)
    {
        BlitSurfaceStandard(src_surface, &src_rect, dest_surface, &dest_rect);
    }
    else
    {
        BlitSurfaceColoured(src_surface, &src_rect, dest_surface, &dest_rect, color);
    }

    return glyph->advance * scale;
}

} /* namespace font */
