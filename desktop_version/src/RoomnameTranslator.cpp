#include "RoomnameTranslator.h"

#include "Constants.h"
#include "Game.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "Map.h"


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

    void overlay_render(void)
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

        if (edit_mode && map.roomname_special)
        {
            graphics.PrintWrap(0, 8, "This is a special room name, which cannot be translated in-game. Please see roomnames_special", 0,192,255, false, 8, 320);
        }
        else if (edit_mode)
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
            if (english_roomname[0] == '\0')
            {
                graphics.bprint(0, 221, "[no roomname]", 0,192,255, true);
            }
            else
            {
                graphics.bprint(0, 221, english_roomname, 0,192,255, true);

                graphics.PrintWrap(0, 8, "This is the first room in the space station that the player is trapped in, so WELCOME ABOARD the space station. This is a reference to the game VVVVVV.", 0,192,255, false, 8, 320);
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

    bool overlay_input(void)
    {
        // Returns true if input "caught" and should not go to gameinput

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

        return edit_mode;
    }
}
