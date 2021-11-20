#include "RoomnameTranslator.h"

#include "Constants.h"
#include "Game.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "Map.h"
#include "UtilityClass.h"


namespace roomname_translator
{
    bool enabled = false;
    bool edit_mode = false;

    int untranslated = 999;

    SDL_Surface* dimbuffer;
    SDL_Rect fullscreen_rect;

    void set_enabled(bool value)
    {
        enabled = value;
        edit_mode = false;
    }

    bool is_pausing(void)
    {
        return enabled && edit_mode;
    }

    void overlay_render(bool* force_roomname_hidden, int* roomname_r, int* roomname_g, int* roomname_b)
    {
        if (edit_mode)
        {
            fullscreen_rect.x = 0;
            fullscreen_rect.y = 0;
            fullscreen_rect.w = 320;
            fullscreen_rect.h = 240;
            SDL_BlitSurface(dimbuffer, NULL, graphics.backBuffer, &fullscreen_rect);
            graphics.bprint(0, 0, "Edit mode [TAB]", 255,255,255);
        }
        else
        {
            graphics.bprint(0, 0, "Play mode [TAB]", 255,255,255);
        }

        char buffer[SCREEN_WIDTH_CHARS + 1];

        SDL_snprintf(buffer, sizeof(buffer), "%3d left", untranslated);
        graphics.bprint(144, 0, buffer, 255,255,255);

        if (map.invincibility)
        {
            graphics.bprint(224, 0, "INV", 255,255,128);
        }

        SDL_snprintf(buffer, sizeof(buffer), "(%2d,%2d)", game.roomx % 100, game.roomy % 100);
        graphics.bprint(320-56, 0, buffer, 255,255,255);

        if (map.roomname_special)
        {
            if (edit_mode)
            {
                graphics.PrintWrap(0, 8, "This is a special room name, which cannot be translated in-game. Please see roomnames_special", 0,192,255, false, 8, 320);
            }
        }
        else
        {
            bool roomname_is_translated = loc::get_roomname_translation(game.roomx, game.roomy)[0] != '\0';

            if (edit_mode)
            {
                const char* english_roomname;
                if (map.finalmode)
                {
                    english_roomname = map.glitchname;
                }
                else
                {
                    english_roomname = map.roomname;
                }
                bool roomname_is_blank = english_roomname[0] == '\0';
                if (roomname_is_blank)
                {
                    graphics.bprint(-1, 221, "[no roomname]", 0,192,255, true);
                    roomname_is_translated = true;
                }
                else
                {
                    graphics.bprint(-1, 221, english_roomname, 0,192,255, true);

                    graphics.PrintWrap(0, 8, "This is the first room in the space station that the player is trapped in, so WELCOME ABOARD the space station. This is a reference to the game VVVVVV.", 0,192,255, false, 8, 320);
                }

                if (key.textentry())
                {
                    *force_roomname_hidden = true;
                    graphics.render_roomname(key.keybuffer.c_str(), 255,255,255);
                    int name_w = graphics.len(key.keybuffer);
                    graphics.bprint((320-name_w)/2+name_w, 231, "_", 255,255,255);
                }
                else if (!roomname_is_translated)
                {
                    *force_roomname_hidden = true;
                    graphics.render_roomname("[no translation]", 255,255,128);
                }
            }
            else if (!roomname_is_translated)
            {
                *roomname_r = 0;
                *roomname_g = 192;
                *roomname_b = 255 - help.glow;
            }
        }
    }


    bool key_pressed_once(SDL_Keycode keyc, bool* held)
    {
        if (key.isDown(keyc))
        {
            if (!*held)
            {
                *held = true;
                return true;
            }
        }
        else
        {
            *held = false;
        }

        return false;
    }

    bool held_tab = false;
    bool held_i = false;
    bool held_escape = false;
    bool held_return = false;
    bool held_e = false;

    bool overlay_input(void)
    {
        // Returns true if input "caught" and should not go to gameinput

        if (key.textentry())
        {
            if (key_pressed_once(SDLK_ESCAPE, &held_escape))
            {
                // Without saving
                key.disabletextentry();
            }

            if (key_pressed_once(SDLK_RETURN, &held_return))
            {
                key.disabletextentry();
                loc::store_roomname_translation(map.custommode, game.roomx, game.roomy, key.keybuffer.c_str());

                if (false)
                {
                    graphics.createtextboxflipme("Translation saved!", -1, 176, 174, 174, 174);
                    graphics.textboxtimer(25);
                }
                else
                {
                    graphics.createtextboxflipme("ERROR: Could not save!", -1, 168, 255, 60, 60);
                    graphics.addline("");
                    graphics.addline("Do the language files exist?");
                    graphics.textboxcenterx();
                    graphics.textboxtimer(50);
                }

                edit_mode = false;
                game.mapheld = true;
            }

            return true;
        }

        if (key_pressed_once(SDLK_TAB, &held_tab))
        {
            edit_mode = !edit_mode;
        }

        if (key_pressed_once(SDLK_i, &held_i))
        {
            if (game.intimetrial)
            {
                // We'll let you enable it in a time trial, but with a twist.
                if (!game.timetrialcheater)
                {
                    game.sabotage_time_trial();
                }
            }
            else if (game.incompetitive())
            {
                return edit_mode;
            }

            map.invincibility = !map.invincibility;
        }

        if (edit_mode)
        {
            if (key_pressed_once(SDLK_ESCAPE, &held_escape))
            {
                edit_mode = false;
                return true;
            }

            if (key_pressed_once(SDLK_RETURN, &held_return) || key_pressed_once(SDLK_e, &held_e))
            {
                if (map.finalmode && map.glitchname[0] == '\0')
                {
                    return true;
                }
                else if (map.roomname[0] == '\0')
                {
                    return true;
                }

                key.enabletextentry();
                key.keybuffer = loc::get_roomname_translation(game.roomx, game.roomy);
            }
        }

        return edit_mode;
    }
}
