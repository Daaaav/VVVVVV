#include <SDL.h>

#include "Constants.h"
#include "Credits.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "FileSystemUtils.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Maths.h"
#include "Music.h"
#include "RoomnameTranslator.h"
#include "Script.h"
#include "UtilityClass.h"
#include "Version.h"

static int tr;
static int tg;
static int tb;

static inline void drawslowdowntext(void)
{
    switch (game.slowdown)
    {
    case 30:
        graphics.PrintWrap( -1, 85, loc::gettext("Game speed is normal."), tr/2, tg/2, tb/2, true);
        break;
    case 24:
        graphics.PrintWrap( -1, 85, loc::gettext("Game speed is at 80%"), tr, tg, tb, true);
        break;
    case 18:
        graphics.PrintWrap( -1, 85, loc::gettext("Game speed is at 60%"), tr, tg, tb, true);
        break;
    case 12:
        graphics.PrintWrap( -1, 85, loc::gettext("Game speed is at 40%"), tr, tg, tb, true);
        break;
    }
}

static void volumesliderrender(void)
{
    char buffer[40 + 1];

    char slider[20 + 1];
    int slider_length;

    const char symbol[] = "[]";
    int symbol_length;

    int offset;
    int num_positions;

    const int* volume_ptr;

    switch (game.currentmenuoption)
    {
    case 0:
        volume_ptr = &music.user_music_volume;
        break;
    case 1:
        volume_ptr = &music.user_sound_volume;
        break;
    default:
        SDL_assert(0 && "Unhandled volume slider menu option!");
        return;
    }

    VVV_fillstring(slider, sizeof(slider), '.');
    slider_length = sizeof(slider) - 1;

    symbol_length = sizeof(symbol) - 1;

    num_positions = slider_length - symbol_length + 1;

    offset = num_positions * (*volume_ptr) / USER_VOLUME_MAX;
    offset = clamp(offset, 0, slider_length - symbol_length);

    /* SDL_strlcpy null-terminates, which would end the string in the middle of
     * it, which we don't want!
     */
    SDL_memcpy(&slider[offset], symbol, symbol_length);

    if (game.slidermode == SLIDER_NONE)
    {
        SDL_strlcpy(buffer, slider, sizeof(buffer));
    }
    else
    {
        /* Draw selection brackets. */
        SDL_snprintf(buffer, sizeof(buffer), "[ %s ]", slider);
    }

    graphics.Print(-1, 95, buffer, tr, tg, tb, true);
}

static void inline drawglitchrunnertext(void)
{
    int tempr = tr;
    int tempg = tg;
    int tempb = tb;

    char buffer[SCREEN_WIDTH_CHARS + 1];

    const enum GlitchrunnerMode mode = GlitchrunnerMode_get();

    if (mode == GlitchrunnerNone)
    {
        tempr /= 2;
        tempg /= 2;
        tempb /= 2;

        SDL_strlcpy(buffer, loc::gettext("Glitchrunner mode is OFF"), sizeof(buffer));
    }
    else
    {
        const char* mode_string = loc::gettext(GlitchrunnerMode_enum_to_string(mode));
        SDL_snprintf(buffer, sizeof(buffer), "Glitchrunner mode is %s", mode_string);
    }

    graphics.PrintWrap(-1, 95, buffer, tempr, tempg, tempb, true);
}

static void menurender(void)
{
    int temp = 50;

    switch (game.currentmenuname)
    {
    case Menu::mainmenu:
    {
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        const char* editionlabel = loc::gettext("MAKE AND PLAY EDITION");
        graphics.Print(264-graphics.len(editionlabel),temp+35,editionlabel,tr, tg, tb);
#endif
#ifdef COMMIT_DATE
        graphics.Print( 310 - (10*8), 210, COMMIT_DATE, tr/2, tg/2, tb/2);
#endif
#ifdef INTERIM_COMMIT
        graphics.Print( 310 - (SDL_arraysize(INTERIM_COMMIT) - 1) * 8, 220, INTERIM_COMMIT, tr/2, tg/2, tb/2);
#endif
        graphics.Print( 310 - (4*8), 230, "v2.4", tr/2, tg/2, tb/2);

        if(music.mmmmmm){
            graphics.Print( 10, 230, loc::gettext("[MMMMMM Mod Installed]"), tr/2, tg/2, tb/2);
        }
        break;
    }
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::levellist:
    {
      if(cl.ListOfMetaData.size()==0){
      graphics.PrintWrap( -1, 90, loc::gettext("ERROR: No levels found."), tr, tg, tb, true);
      }
      int tmp=game.currentmenuoption+(game.levelpage*8);
      if(INBOUNDS_VEC(tmp, cl.ListOfMetaData)){
        const bool nextlastoptions = cl.ListOfMetaData.size() > 8;
        //Don't show next/previous page or return to menu options here!
        if(nextlastoptions && game.menuoptions.size() - game.currentmenuoption<=3){

        }else{
          graphics.bigprint( -1, 15, cl.ListOfMetaData[tmp].title, tr, tg, tb, true);
          char creatorline[SCREEN_WIDTH_CHARS + 1];
          SDL_snprintf(
            creatorline, sizeof(creatorline),
            loc::gettext("by %s"),
            cl.ListOfMetaData[tmp].creator.c_str()
          );
          graphics.Print( -1, 40, creatorline, tr, tg, tb, true);
          graphics.Print( -1, 50, cl.ListOfMetaData[tmp].website, tr, tg, tb, true);
          graphics.Print( -1, 70, cl.ListOfMetaData[tmp].Desc1, tr, tg, tb, true);
          graphics.Print( -1, 80, cl.ListOfMetaData[tmp].Desc2, tr, tg, tb, true);
          graphics.Print( -1, 90, cl.ListOfMetaData[tmp].Desc3, tr, tg, tb, true);
        }
      }
      break;
    }
#endif
    case Menu::errornostart:
        graphics.PrintWrap( -1, 65, loc::gettext("ERROR: This level has no start point!"), tr, tg, tb, true);
        break;
    case Menu::gameplayoptions:
    {
        int gameplayoptionsoffset = 0;
#if !defined(MAKEANDPLAY)
        if (game.ingame_titlemode && game.unlock[18])
#endif
        {
            gameplayoptionsoffset = 1;
            if (game.currentmenuoption == 0) {
                graphics.bigprint( -1, 30, loc::gettext("Flip Mode"), tr, tg, tb, true);
                graphics.PrintWrap( -1, 65, loc::gettext("Flip the entire game vertically."), tr, tg, tb, true);
                if (graphics.setflipmode)
                {
                    graphics.PrintWrap( -1, 85, loc::gettext("Currently ENABLED!"), tr, tg, tb, true);
                }
                else
                {
                    graphics.PrintWrap( -1, 85, loc::gettext("Currently Disabled."), tr/2, tg/2, tb/2, true);
                }
            }
        }

        if (game.currentmenuoption == gameplayoptionsoffset + 0)
        {
            //Toggle FPS
            graphics.bigprint(-1, 30, loc::gettext("Toggle 30+ FPS"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Change whether the game runs at 30 or over 30 FPS."), tr, tg, tb, true);

            if (!game.over30mode)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Current mode: 30 FPS"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Current mode: Over 30 FPS"), tr, tg, tb, true);
            }
            break;
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 1)
        {
            //Speedrunner options
            graphics.bigprint(-1, 30, loc::gettext("Speedrunner Options"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Access some advanced settings that might be of interest to speedrunners."), tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 2)
        {
            //Advanced options
            graphics.bigprint(-1, 30, loc::gettext("Advanced Options"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("All other gameplay settings."), tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 3)
        {
            //Clear Data
            graphics.bigprint(-1, 30, loc::gettext("Clear Data"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Delete your main game save data and unlocked play modes."), tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 4)
        {
            graphics.bigprint(-1, 30, loc::gettext("Clear Data"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Delete your custom level save data and completion stars."), tr, tg, tb, true);
        }

        break;
    }
    case Menu::options:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint(-1, 30, loc::gettext("Gameplay Options"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Adjust various gameplay settings."), tr, tg, tb, true);
            break;
        case 1:
            graphics.bigprint(-1, 30, loc::gettext("Graphics Options"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Adjust screen settings."), tr, tg, tb, true);
            break;
        case 2:
            graphics.bigprint(-1, 30, loc::gettext("Audio Options"), tr, tg, tb, true);
            if (music.mmmmmm)
            {
                graphics.PrintWrap(-1, 65, loc::gettext("Adjust volume settings and soundtrack."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 65, loc::gettext("Adjust volume settings."), tr, tg, tb, true);
            }
            break;
        case 3:
            graphics.bigprint(-1, 30, loc::gettext("Game Pad Options"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Rebind your controller's buttons and adjust sensitivity."), tr, tg, tb, true);
            break;
        case 4:
            graphics.bigprint(-1, 30, loc::gettext("Accessibility"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Disable screen effects, enable slowdown modes or invincibility."), tr, tg, tb, true);
            break;
        case 5:
            graphics.bigprint( -1, 30, loc::gettext("Language"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Change the language."), tr, tg, tb, true);
        }
        break;
    case Menu::graphicoptions:
        if (graphics.screenbuffer == NULL)
        {
            SDL_assert(0 && "Screenbuffer is NULL!");
            break;
        }

        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Toggle Fullscreen"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Change to fullscreen/windowed mode."), tr, tg, tb, true);

            if (graphics.screenbuffer->isWindowed)
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: WINDOWED"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: FULLSCREEN"), tr, tg, tb, true);
            }
            break;

        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Scaling Mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Choose letterbox/stretch/integer mode."), tr, tg, tb, true);

            switch (graphics.screenbuffer->stretchMode)
            {
            case 2:
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: INTEGER"), tr, tg, tb, true);
                break;
            case 1:
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: STRETCH"), tr, tg, tb, true);
                break;
            default:
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: LETTERBOX"), tr, tg, tb, true);
                break;
            }
            break;
        case 2:
            graphics.bigprint(-1, 30, loc::gettext("Resize to Nearest"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Resize to the nearest window size that is of an integer multiple."), tr, tg, tb, true);
            if (!graphics.screenbuffer->isWindowed)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("You must be in windowed mode to use this option."), tr, tg, tb, true);
            }
            break;
        case 3:
            graphics.bigprint( -1, 30, loc::gettext("Toggle Filter"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Change to nearest/linear filter."), tr, tg, tb, true);

            if (graphics.screenbuffer->isFiltered)
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: LINEAR"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Current mode: NEAREST"), tr, tg, tb, true);
            }
            break;

        case 4:
            graphics.bigprint( -1, 30, loc::gettext("Analogue Mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("There is nothing wrong with your television set. Do not attempt to adjust the picture."), tr, tg, tb, true);
            break;
        case 5:
            graphics.bigprint(-1, 30, loc::gettext("Toggle VSync"), tr, tg, tb, true);
            /* FIXME: Upgrade to SDL 2.0.18 and remove this ifdef when it releases! */
#if SDL_VERSION_ATLEAST(2, 0, 17)
            graphics.PrintWrap(-1, 65, loc::gettext("Turn VSync on or off."), tr, tg, tb, true);
#else
            graphics.Print(-1, 65, "Your SDL version is too old!", tr, tg, tb, true);
            graphics.Print(-1, 75, "Edit the config file.", tr, tg, tb, true);
#endif

            if (!graphics.screenbuffer->vsync)
            {
                graphics.PrintWrap(-1, 85, loc::gettext("Current mode: VSYNC OFF"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, 85, loc::gettext("Current mode: VSYNC ON"), tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::audiooptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint(-1, 30, loc::gettext("Music Volume"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Change the volume of the music."), tr, tg, tb, true);
            volumesliderrender();
            break;
        case 1:
            graphics.bigprint(-1, 30, loc::gettext("Sound Volume"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Change the volume of sound effects."), tr, tg, tb, true);
            volumesliderrender();
            break;
        case 2:
            if (!music.mmmmmm)
            {
                break;
            }
        {
            char buffer[SCREEN_WIDTH_CHARS + 1];
            char soundtrack[6 + 1];
            char letter;
            if (music.usingmmmmmm)
            {
                letter = 'M';
            }
            else
            {
                letter = 'P';
            }
            VVV_fillstring(soundtrack, sizeof(soundtrack), letter);
            SDL_snprintf(buffer, sizeof(buffer), loc::gettext("Current soundtrack: %s"), soundtrack);

            graphics.bigprint(-1, 30, loc::gettext("Soundtrack"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Toggle between MMMMMM and PPPPPP."), tr, tg, tb, true);
            graphics.PrintWrap(-1, 85, buffer, tr, tg, tb, true);
            break;
        }

        }
        break;
    case Menu::credits:
        graphics.Print( -1, 50, loc::gettext("VVVVVV is a game by"), tr, tg, tb, true);
        graphics.bigprint( 40, 65, loc::gettext("Terry Cavanagh"), tr, tg, tb, true, 2);

        graphics.drawimagecol(7, -1, 86, true);

        graphics.Print( -1, 120, loc::gettext("and features music by"), tr, tg, tb, true);
        graphics.bigprint( 40, 135, loc::gettext("Magnus P~lsson"), tr, tg, tb, true, 2);
        graphics.drawimagecol(8, -1, 156, true);
        break;
    case Menu::credits2:
        graphics.Print( -1, 50, loc::gettext("Roomnames are by"), tr, tg, tb, true);
        graphics.bigprint( 40, 65, loc::gettext("Bennett Foddy"), tr, tg, tb, true);
        graphics.drawimagecol(9, -1, 86, true);
        graphics.Print( -1, 110, loc::gettext("C++ version by"), tr, tg, tb, true);
        graphics.bigprint( 40, 125, loc::gettext("Simon Roth"), tr, tg, tb, true);
        graphics.bigprint( 40, 145, loc::gettext("Ethan Lee"), tr, tg, tb, true);
        graphics.bigprint( 40, 165, loc::gettext("Misa Kai"), tr, tg, tb, true);
        break;
    case Menu::credits25:
        graphics.Print( -1, 40, loc::gettext("Beta Testing by"), tr, tg, tb, true);
        graphics.bigprint( 40, 55, loc::gettext("Sam Kaplan"), tr, tg, tb, true);
        graphics.bigprint( 40, 75, loc::gettext("Pauli Kohberger"), tr, tg, tb, true);
        graphics.Print( -1, 130, loc::gettext("Ending Picture by"), tr, tg, tb, true);
        graphics.bigprint( 40, 145, loc::gettext("Pauli Kohberger"), tr, tg, tb, true);
        break;
    case Menu::credits3:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("VVVVVV is supported by the following patrons"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = VVV_min(startidx + 9, (int)SDL_arraysize(Credits::superpatrons));

        int xofs = 80 - 16;
        int yofs = 40 + 20;

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(xofs, yofs, Credits::superpatrons[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits4:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("and also by"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = VVV_min(startidx + 14, (int)SDL_arraysize(Credits::patrons));

        int maxheight = 10 * 14;
        int totalheight = (endidx - startidx) * 10;
        int emptyspace = maxheight - totalheight;

        int yofs = 40 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(80, yofs, Credits::patrons[i], tr, tg, tb);
            yofs += 10;
        }
        break;
    }
    case Menu::credits5:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("With contributions on GitHub from"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = VVV_min(startidx + 9, (int)SDL_arraysize(Credits::githubfriends));

        int maxheight = 14 * 9;
        int totalheight = (endidx - startidx) * 14;
        int emptyspace = maxheight - totalheight;

        int xofs, yofs;

        xofs = 80 - 16;
        yofs = 40 + 20 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(xofs, yofs, Credits::githubfriends[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits6:
        graphics.PrintWrap( -1, 20, loc::gettext("and thanks also to:"), tr, tg, tb, true);

        graphics.bigprint(80, 60, loc::gettext("You!"), tr, tg, tb, true);

        graphics.PrintWrap( 80, 100, loc::gettext("Your support makes it possible for me to continue making the games I want to make, now and into the future."), tr, tg, tb,true);

        graphics.PrintWrap( 80, 150,loc::gettext("Thank you!"), tr, tg, tb,true);
        break;
    case Menu::setinvincibility:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure you want to enable invincibility?"), tr, tg, tb, true);
        break;
    case Menu::setslowdown:
        graphics.bigprint( -1, 30, loc::gettext("Game Speed"), tr, tg, tb, true);
        graphics.PrintWrap( -1, 65, loc::gettext("Select a new game speed below."), tr, tg, tb, true);
        drawslowdowntext();
        break;
    case Menu::newgamewarning:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure? This will delete your current saves..."), tr, tg, tb, true);
        break;
    case Menu::cleardatamenu:
    case Menu::clearcustomdatamenu:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure you want to delete all your saved data?"), tr, tg, tb, true);
        break;
    case Menu::deletequicklevel:
        graphics.PrintWrap(-1, 100, loc::gettext("Are you sure you want to delete your quicksave?"), tr, tg, tb, true);
        break;
    case Menu::startnodeathmode:
        graphics.PrintWrap( -1, 45, loc::gettext("Good luck!"), tr, tg, tb, true);
        graphics.PrintWrap( -1, 80, loc::gettext("You cannot save in this mode."), tr, tg, tb, true);
        graphics.PrintWrap( -1, 105, loc::gettext("Would you like to disable the cutscenes during the game?"), tr, tg, tb, true);
        break;
    case Menu::controller:
        graphics.bigprint( -1, 30, loc::gettext("Game Pad"), tr, tg, tb, true);
        graphics.PrintWrap( -1, 55, loc::gettext("Change controller options."), tr, tg, tb, true);
        switch (game.currentmenuoption)
        {
        case 0:
            //graphics.Print( -1, 85, " Low     Medium     High", tr, tg, tb, true);
            graphics.Print(32, 75, loc::gettext("Low"), tr, tg, tb);
            graphics.Print(-1, 75, loc::gettext("Medium"), tr, tg, tb, true);
            #define HIGHLABEL loc::gettext("High")
            graphics.Print(288-graphics.len(HIGHLABEL), 75, HIGHLABEL, tr, tg, tb);
            #undef HIGHLABEL
            switch(key.sensitivity)
            {
            case 0:
                graphics.Print( -1, 85, "[]..........................", tr, tg, tb, true);
                break;
            case 1:
                graphics.Print( -1, 85, ".......[]...................", tr, tg, tb, true);
                break;
            case 2:
                graphics.Print( -1, 85, ".............[].............", tr, tg, tb, true);
                break;
            case 3:
                graphics.Print( -1, 85, "...................[].......", tr, tg, tb, true);
                break;
            case 4:
                graphics.Print( -1, 85, "..........................[]", tr, tg, tb, true);
                break;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            graphics.Print( -1, 75, loc::gettext("Flip is bound to: ") + std::string(help.GCString(game.controllerButton_flip)) , tr, tg, tb, true);
            graphics.Print( -1, 85, loc::gettext("Enter is bound to: ")  + std::string(help.GCString(game.controllerButton_map)), tr, tg, tb, true);
            graphics.Print( -1, 95, loc::gettext("Menu is bound to: ") + std::string(help.GCString(game.controllerButton_esc)) , tr, tg, tb, true);
            graphics.Print( -1, 105, loc::gettext("Restart is bound to: ") + std::string(help.GCString(game.controllerButton_restart)) , tr, tg, tb, true);
            graphics.Print( -1, 115, loc::gettext("Interact is bound to: ") + std::string(help.GCString(game.controllerButton_interact)) , tr, tg, tb, true);
            break;
        }


        break;
    case Menu::language:
        if (loc::languagelist.empty())
        {
            graphics.PrintWrap(-1, 90, loc::gettext("ERROR: No language files found."), tr, tg, tb, true);
        }
        else if ((unsigned)game.currentmenuoption < loc::languagelist.size())
        {
            graphics.PrintWrap(-1, 15, loc::languagelist[game.currentmenuoption].credit, tr, tg, tb, true);
        }
        break;
    case Menu::translator_main:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Translator options"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Some options that are useful for translators and developers."), tr, tg, tb, true);
            break;
        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Maintenance"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Sync all language files after adding new strings, and check whether everything can be translated."), tr, tg, tb, true);
            break;
        }
        {
            if (FILESYSTEM_isMainLangDirFromRepo())
            {
                // Just giving manual compilers some hint as to why this menu is here!
                graphics.Print(8, 208, loc::gettext("Repository language folder:"), tr/2, tg/2, tb/2);
            }
            else
            {
                graphics.Print(8, 208, loc::gettext("Language folder:"), tr/2, tg/2, tb/2);
            }

            char* mainLangDir = FILESYSTEM_getUserMainLangDirectory();
            graphics.Print(316-graphics.len(mainLangDir), 224, mainLangDir, tr/2, tg/2, tb/2);
        }
        break;
    case Menu::translator_options:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Statistics"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Count the amount of untranslated strings for this language."), tr, tg, tb, true);
            break;
        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Translate rooms"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Enable room name translation mode, so you can translate room names in context."), tr, tg, tb, true);

            if (roomname_translator::enabled)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Currently ENABLED!"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Currently Disabled."), tr/2, tg/2, tb/2, true);
            }
            break;
        case 2:
            graphics.bigprint( -1, 30, loc::gettext("Menu test"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Cycle through most menus in the game. The menus will not actually work, all options take you to the next menu instead. Press Escape to stop."), tr, tg, tb, true);
            break;
        case 3:
            graphics.bigprint( -1, 30, loc::gettext("Limits check"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Find translations that don't fit within their defined bounds."), tr, tg, tb, true);
            break;
        }
        break;
    case Menu::translator_maintenance:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Sync language files"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Merge all new strings from the template files into the translation files, keeping existing translations."), tr, tg, tb, true);
            break;
        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Statistics"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Count the amount of untranslated strings for each language."), tr, tg, tb, true);
            break;
        case 2:
            graphics.bigprint( -1, 30, loc::gettext("Test mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Show all translatable strings as a V or X, revealing untranslatable or forgotten strings. X means missing from file."), tr, tg, tb, true);
        }
        break;
    case Menu::translator_maintenance_sync:
        graphics.PrintWrap(-1, 80, loc::gettext("If new strings were added to the English template language files, this feature will insert them in the translation files for all languages. Make a backup, just in case."), tr, tg, tb, true);
        break;
    case Menu::speedrunneroptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint(-1, 30, loc::gettext("Glitchrunner Mode"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Re-enable glitches that existed in previous versions of the game."), tr, tg, tb, true);
            drawglitchrunnertext();
            break;
        case 1:
            graphics.bigprint(-1, 30, loc::gettext("Input Delay"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Re-enable the 1-frame input delay from previous versions of the game."), tr, tg, tb, true);
            if (game.inputdelay)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Input delay is ON"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Input delay is OFF"), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case 2:
        {
            char buffer[SCREEN_WIDTH_CHARS + 1];
            const char* button;

            graphics.bigprint(-1, 30, loc::gettext("Interact Button"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Toggle whether you interact with prompts using ENTER or E."), tr, tg, tb, true);

            if (game.separate_interact)
            {
                button = loc::gettext("E");
            }
            else
            {
                button = loc::gettext("ENTER");
            }

            SDL_snprintf(buffer, sizeof(buffer), loc::gettext("Interact button: %s"), button);
            graphics.PrintWrap(-1, 95, buffer, tr, tg, tb, true);
            break;
        }
        case 3:
            graphics.bigprint(-1, 30, loc::gettext("Fake Load Screen"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Disable the fake loading screen which appears on game launch."), tr, tg, tb, true);
            if (game.skipfakeload)
                graphics.PrintWrap(-1, 95, loc::gettext("Fake loading screen is OFF"), tr / 2, tg / 2, tb / 2, true);
            else
                graphics.PrintWrap(-1, 95, loc::gettext("Fake loading screen is ON"), tr, tg, tb, true);
            break;
        case 4:
            graphics.bigprint(-1, 30, loc::gettext("In-Game Timer"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Toggle the in-game timer outside of time trials."), tr, tg, tb, true);
            if (game.showingametimer)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("In-Game Timer is ON"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("In-Game Timer is OFF"), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        break;
    case Menu::setglitchrunner:
        graphics.bigprint(-1, 30, loc::gettext("Glitchrunner Mode"), tr, tg, tb, true);
        graphics.PrintWrap(-1, 65, loc::gettext("Select a new glitchrunner version below."), tr, tg, tb, true);
        drawglitchrunnertext();
        break;
    case Menu::advancedoptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Unfocus Pause"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Toggle if the game will pause when the window is unfocused."), tr, tg, tb, true);
            if (game.disablepause)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Unfocus pause is OFF"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Unfocus pause is ON"), tr, tg, tb, true);
            }
            break;
        case 1:
            graphics.bigprint(-1, 30, loc::gettext("Unfocus Audio"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Toggle if the audio will pause when the window is unfocused."), tr, tg, tb, true);
            if (game.disableaudiopause)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Unfocus audio pause is OFF"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Unfocus audio pause is ON"), tr, tg, tb, true);
            }
            break;
        case 2:
            graphics.bigprint(-1, 30, loc::gettext("Room Name BG"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Lets you see through what is behind the name at the bottom of the screen."), tr, tg, tb, true);
            if (graphics.translucentroomname)
                graphics.PrintWrap(-1, 95, loc::gettext("Room name background is TRANSLUCENT"), tr/2, tg/2, tb/2, true);
            else
                graphics.PrintWrap(-1, 95, loc::gettext("Room name background is OPAQUE"), tr, tg, tb, true);
            break;
        }
        break;
    case Menu::accessibility:
    {
#ifdef MAKEANDPLAY
 #define OFFSET 0
#else
 #define OFFSET 1
#endif

        switch (game.currentmenuoption)
        {
#if !defined(MAKEANDPLAY)
        case 0:
            graphics.bigprint(-1, 30, loc::gettext("Unlock Play Modes"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Unlock parts of the game normally unlocked as you progress."), tr, tg, tb, true);
            break;
#endif
        case OFFSET+0:
            graphics.bigprint(-1, 30, loc::gettext("Invincibility"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Explore the game freely without dying. (Can cause glitches.)"), tr, tg, tb, true);
            if (map.invincibility)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Invincibility is ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Invincibility is OFF."), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case OFFSET+1:
            graphics.bigprint(-1, 30, loc::gettext("Slowdown"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Reduce the game speed."), tr, tg, tb, true);
            drawslowdowntext();
            break;
        case OFFSET+2:
            graphics.bigprint(-1, 30, loc::gettext("Backgrounds"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Disable animated backgrounds in menus and during gameplay."), tr, tg, tb, true);
            if (!game.colourblindmode)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Backgrounds are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Backgrounds are OFF."), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case OFFSET+3:
            graphics.bigprint(-1, 30, loc::gettext("Screen Effects"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Disables screen shakes and flashes."), tr, tg, tb, true);
            if (!game.noflashingmode)
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Screen Effects are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 95, loc::gettext("Screen Effects are OFF."), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case OFFSET+4:
        {
            const char* text;

            graphics.bigprint(-1, 30, loc::gettext("Text Outline"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Disables outline on game text."), tr, tg, tb, true);

            FillRect(graphics.backBuffer, 0, 84, 320, 10, tr, tg, tb);

            if (!graphics.notextoutline)
            {
                text = loc::gettext("Text outlines are ON.");
            }
            else
            {
                text = loc::gettext("Text outlines are OFF.");
            }

            graphics.bprint(-1, 85, text, 255, 255, 255, true);
            break;
        }

        }
        break;

#undef OFFSET
    }
    case Menu::playint1:
    case Menu::playint2:
        graphics.PrintWrap( -1, 65, loc::gettext("Who do you want to play the level with?"), tr, tg, tb, true);
        break;
    case Menu::playmodes:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Time Trials"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Replay any level in the game in a competitive time trial mode."), tr, tg, tb, true);

            if (game.nocompetitive())
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Time Trials are not available with slowdown or invincibility."), tr, tg, tb, true);
            }
            break;
        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Intermissions"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Replay the intermission levels."), tr, tg, tb, true);

            if (!game.unlock[15] && !game.unlock[16])
            {
                graphics.PrintWrap( -1, 95, loc::gettext("TO UNLOCK: Complete the intermission levels in-game."), tr, tg, tb, true);
            }
            break;
        case 2:
            graphics.bigprint( -1, 30, loc::gettext("No Death Mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Play the entire game without dying once."), tr, tg, tb, true);

            if (game.nocompetitive())
            {
                graphics.PrintWrap( -1, 105, loc::gettext("No Death Mode is not available with slowdown or invincibility."), tr, tg, tb, true);
            }
            else if (!game.unlock[17])
            {
                graphics.PrintWrap( -1, 105, loc::gettext("TO UNLOCK: Achieve an S-rank or above in at least 4 time trials."), tr, tg, tb, true);
            }
            break;
        case 3:
            // WARNING: Partially duplicated in Menu::options
            graphics.bigprint( -1, 30, loc::gettext("Flip Mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Flip the entire game vertically. Compatible with other game modes."), tr, tg, tb, true);

            if (game.unlock[18])
            {
                if (graphics.setflipmode)
                {
                    graphics.PrintWrap( -1, 105, loc::gettext("Currently ENABLED!"), tr, tg, tb, true);
                }
                else
                {
                    graphics.PrintWrap( -1, 105, loc::gettext("Currently Disabled."), tr/2, tg/2, tb/2, true);
                }
            }
            else
            {
                graphics.PrintWrap( -1, 105, loc::gettext("TO UNLOCK: Complete the game."), tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::youwannaquit:
        graphics.PrintWrap( -1, 75, loc::gettext("Are you sure you want to quit?"), tr, tg, tb, true);
        break;
    case Menu::continuemenu:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            //Show teleporter save info
            graphics.drawpixeltextbox(17, 65-20, 286, 90, 65, 185, 207);

            graphics.bigprint(-1, 20, loc::gettext("Tele Save"), tr, tg, tb, true);
            graphics.Print(0, 80-20, game.tele_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true); // TODO LOC. Should translate only at presentation (here), not save to save file, to accomodate language changes. Repeat below
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.tele_crewstats[i], true);
            }
            graphics.Print(59, 132-20, game.tele_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            const std::string& trinketcount = help.number_words(game.tele_trinkets);
            graphics.Print(262-graphics.len(trinketcount), 132-20, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.drawsprite(34, 126-20, 50, graphics.col_clock);
            graphics.drawsprite(270, 126-20, 22, graphics.col_trinket);
            break;
        }
        case 1:
        {
            //Show quick save info
            graphics.drawpixeltextbox(17, 65-20, 286, 90, 65, 185, 207);

            graphics.bigprint(-1, 20, loc::gettext("Quick Save"), tr, tg, tb, true);
            graphics.Print(0, 80-20, game.quick_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.quick_crewstats[i], true);
            }
            graphics.Print(59, 132-20, game.quick_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            const std::string& trinketcount = help.number_words(game.quick_trinkets);
            graphics.Print(262-graphics.len(trinketcount), 132-20, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.drawsprite(34, 126-20, 50, graphics.col_clock);
            graphics.drawsprite(270, 126-20, 22, graphics.col_trinket);
            break;
        }
        }
        break;
    case Menu::gameover:
    case Menu::gameover2:
    {
        graphics.bigprint( -1, 25, loc::gettext("GAME OVER"), tr, tg, tb, true, 3);

        for (size_t i = 0; i < SDL_arraysize(game.ndmresultcrewstats); i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.ndmresultcrewstats[i], true);
        }
        char buffer[SCREEN_WIDTH_CHARS + 1];
        loc::gettext_plural_fill(buffer, sizeof(buffer), "You rescued %s crewmates", "You rescued %s crewmate", game.ndmresultcrewrescued);
        graphics.Print(0, 100, buffer, tr, tg, tb, true);

        loc::gettext_plural_fill(buffer, sizeof(buffer), "and found %s trinkets.", "and found %s trinket.", game.ndmresulttrinkets);
        graphics.Print(0, 110, buffer, tr, tg, tb, true);

        graphics.Print(0, 145, loc::gettext("You managed to reach:"), tr, tg, tb, true);
        graphics.Print(0, 155, game.ndmresulthardestroom, tr, tg, tb, true);

        const char* encouragement;
        switch (game.ndmresultcrewrescued)
        {
        case 1:
            encouragement = loc::gettext("Keep trying! You'll get there!");
            break;
        case 2:
            encouragement = loc::gettext("Nice one!");
            break;
        case 3:
            encouragement = loc::gettext("Wow! Congratulations!");
            break;
        case 4:
            encouragement = loc::gettext("Incredible!");
            break;
        case 5:
            encouragement = loc::gettext("Unbelievable! Well done!");
            break;
        case 6:
            encouragement = loc::gettext("Er, how did you do that?");
            break;
        }

        graphics.PrintWrap(0, 190, encouragement, tr, tg, tb, true);
        break;
    }
    case Menu::nodeathmodecomplete:
    case Menu::nodeathmodecomplete2:
    {
        graphics.bigprint( -1, 8, loc::gettext("WOW"), tr, tg, tb, true, 4);

        for (size_t i = 0; i < SDL_arraysize(game.ndmresultcrewstats); i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.ndmresultcrewstats[i], true);
        }
        graphics.Print(0, 100, loc::gettext("You rescued all the crewmates!"), tr, tg, tb, true);

        char buffer[SCREEN_WIDTH_CHARS + 1];
        loc::gettext_plural_fill(buffer, sizeof(buffer), "And you found %s trinkets.", "And you found %s trinket.", game.ndmresulttrinkets);
        graphics.PrintWrap(0, 110, buffer, tr, tg, tb, true);

        graphics.PrintWrap(0, 160, loc::gettext("A new trophy has been awarded and placed in the secret lab to acknowledge your achievement!"), tr, tg, tb, true);
        break;
    }
    case Menu::timetrialcomplete:
    case Menu::timetrialcomplete2:
    case Menu::timetrialcomplete3:
    {
        graphics.bigprint( -1, 20, loc::gettext("Results"), tr, tg, tb, true, 3);

        std::string tempstring = game.resulttimestring() + " / " + game.timetstring(game.timetrialresultpar) + ".99"; // TODO LOC localize like said in Game.cpp?

        graphics.drawspritesetcol(22, 80-15, 50, 22);
        graphics.Print(49, 80-15, loc::gettext("TIME TAKEN:"), 255, 255, 255);
        graphics.Print(49, 90-15, tempstring, tr, tg, tb);
        if (game.timetrialresulttime <= game.timetrialresultpar)
        {
            graphics.Print(228, 90-15, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        tempstring = help.String(game.timetrialresultdeaths);
        graphics.drawspritesetcol(22-4, 80+20-4, 12, 22);
        graphics.Print(49, 80+20, loc::gettext("NUMBER OF DEATHS:"), 255, 255, 255);
        graphics.Print(49, 90+20, tempstring, tr, tg, tb);
        if (game.timetrialresultdeaths == 0)
        {
            graphics.Print(228, 90+20, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        char buffer[SCREEN_WIDTH_CHARS + 1];
        SDL_snprintf(
            buffer, sizeof(buffer),
            loc::gettext("%d of %d"),
            game.timetrialresulttrinkets, game.timetrialresultshinytarget
        );
        graphics.drawspritesetcol(22, 80+55, 22, 22);
        graphics.Print(49, 80+55, loc::gettext("SHINY TRINKETS:"), 255, 255, 255);
        graphics.Print(49, 90+55, buffer, tr, tg, tb);
        if (game.timetrialresulttrinkets >= game.timetrialresultshinytarget)
        {
            graphics.Print(228, 90+55, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        const char* rank = "";
        switch(game.timetrialrank)
        {
        case 0:
            rank = loc::gettext("B");
            break;
        case 1:
            rank = loc::gettext("A");
            break;
        case 2:
            rank = loc::gettext("S");
            break;
        case 3:
            rank = loc::gettext("V");
            break;
        }
        int rankw = graphics.len(rank)*4;
        int ranktextw = graphics.len(loc::gettext("Rank:"))*2 + 16 + rankw;
        int ranktextx = (320-ranktextw)/2;
        int rankx = ranktextx + ranktextw - rankw;
        if (game.currentmenuname == Menu::timetrialcomplete2 || game.currentmenuname == Menu::timetrialcomplete3)
        {
            graphics.bigprint(ranktextx, 175, loc::gettext("Rank:"), tr, tg, tb, false, 2);
        }

        if (game.currentmenuname == Menu::timetrialcomplete3)
        {
            graphics.bigprint(rankx, 165, rank, 255, 255, 255, false, 4);
        }
        break;
    }
    case Menu::unlockmenutrials:
        graphics.bigprint( -1, 30, loc::gettext("Unlock Time Trials"), tr, tg, tb, true);
        graphics.PrintWrap( -1, 65, loc::gettext("You can unlock each time trial separately."), tr, tg, tb, true);
        break;
    case Menu::timetrials:
        switch (game.currentmenuoption)
        {
        case 0:
            // TODO LOC. Context string for not globally limiting area names to 20 characters?
            // In fact, this entire switch statement is hairy as for translation.
            // Too tight for other languages, probably needs repositionings, and everything is hardcoded 6 times.
            if(game.unlock[9])
            {
                graphics.bigprint( -1, 30, "Space Station 1", tr, tg, tb, true);
                if (game.besttimes[0] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[0]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[0])+"/2", tr, tg, tb);
                    graphics.Print( 110, 85,help.String(game.bestlives[0]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    1:15", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[0])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Violet", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find three trinkets", tr, tg, tb, true);
            }
            break;
        case 1:
            if(game.unlock[10])
            {
                graphics.bigprint( -1, 30, "The Laboratory", tr, tg, tb, true);
                if (game.besttimes[1] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[1]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[1])+"/4", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[1]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    2:45", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[1])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Victoria", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find six trinkets", tr, tg, tb, true);
            }
            break;
        case 2:
            if(game.unlock[11])
            {
                graphics.bigprint( -1, 30, "The Tower", tr, tg, tb, true);
                if (game.besttimes[2] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[2]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[2])+"/2", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[2]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    1:45", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[2])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Vermilion", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find nine trinkets", tr, tg, tb, true);
            }
            break;
        case 3:
            if(game.unlock[12])
            {
                graphics.bigprint( -1, 30, "Space Station 2", tr, tg, tb, true);
                if (game.besttimes[3] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[3]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[3])+"/5", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[3]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    3:20", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[3])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Vitellary", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find twelve trinkets", tr, tg, tb, true);
            }
            break;
        case 4:
            if(game.unlock[13])
            {
                graphics.bigprint( -1, 30, "The Warp Zone", tr, tg, tb, true);
                if (game.besttimes[4] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[4]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[4])+"/1", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[4]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    2:00", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[4])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Verdigris", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find fifteen trinkets", tr, tg, tb, true);
            }
            break;
        case 5:
            if(game.unlock[14])
            {
                graphics.bigprint( -1, 30, "The Final Level", tr, tg, tb, true);
                if (game.besttimes[5] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[5]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[5])+"/1", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[5]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    2:15", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[5])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Complete the game", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find eighteen trinkets", tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::gamecompletecontinue:
        graphics.bigprint( -1, 25, loc::gettext("Congratulations!"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 45, loc::gettext("Your save files have been updated."), tr, tg, tb, true);

        graphics.PrintWrap( -1, 110, loc::gettext("If you want to keep exploring the game, select CONTINUE from the play menu."), tr, tg, tb, true);
        break;
    case Menu::unlockmenu:
        graphics.bigprint( -1, 30, loc::gettext("Unlock Play Modes"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 65, loc::gettext("From here, you may unlock parts of the game that are normally unlocked as you play."), tr, tg, tb, true);
        break;
    case Menu::unlocktimetrial:
        graphics.bigprint( -1, 45, loc::gettext("Congratulations!"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked a new Time Trial."), tr, tg, tb, true);
        break;
    case Menu::unlocktimetrials:
        graphics.bigprint( -1, 45, loc::gettext("Congratulations!"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked some new Time Trials."), tr, tg, tb, true);
        break;
    case Menu::unlocknodeathmode:
        graphics.bigprint( -1, 45, loc::gettext("Congratulations!"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked No Death Mode."), tr, tg, tb, true);
        break;
    case Menu::unlockflipmode:
        graphics.bigprint( -1, 45, loc::gettext("Congratulations!"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked Flip Mode."), tr, tg, tb, true);
        break;
    case Menu::unlockintermission:
        graphics.bigprint( -1, 45, loc::gettext("Congratulations!"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked the intermission levels."), tr, tg, tb, true);
        break;
    case Menu::playerworlds:
    {
        std::string tempstring = FILESYSTEM_getUserLevelDirectory();
        if(tempstring.length()>80){
            graphics.PrintWrap( -1, 160, loc::gettext("To install new player levels, copy the .vvvvvv files to this folder:"), tr, tg, tb, true);
            graphics.Print( 320-((tempstring.length()-80)*8), 190, tempstring.substr(0,tempstring.length()-80), tr, tg, tb);
            graphics.Print( 0, 200, tempstring.substr(tempstring.length()-80,40), tr, tg, tb);
            graphics.Print( 0, 210, tempstring.substr(tempstring.length()-40,40), tr, tg, tb);
        }else if(tempstring.length()>40){
            graphics.PrintWrap( -1, 170, loc::gettext("To install new player levels, copy the .vvvvvv files to this folder:"), tr, tg, tb, true);
            graphics.Print( 320-((tempstring.length()-40)*8), 200, tempstring.substr(0,tempstring.length()-40), tr, tg, tb);
            graphics.Print( 0, 210, tempstring.substr(tempstring.length()-40,40), tr, tg, tb);
        }else{
            graphics.PrintWrap( -1, 180, loc::gettext("To install new player levels, copy the .vvvvvv files to this folder:"), tr, tg, tb, true);
            graphics.Print( 320-(tempstring.length()*8), 210, tempstring, tr, tg, tb);
        }
        break;
    }
    case Menu::errorsavingsettings:
        graphics.PrintWrap( -1, 95, loc::gettext("ERROR: Could not save settings file!"), tr, tg, tb, true);
        break;
    case Menu::errorloadinglevel:
    {
        const char* str;
        if (FILESYSTEM_levelDirHasError())
        {
            str = FILESYSTEM_getLevelDirError();
        }
        else
        {
            str = graphics.error;
        }
        graphics.bigprint(-1, 45, loc::gettext("ERROR"), tr, tg, tb, true);
        graphics.PrintWrap(-1, 65, str, tr, tg, tb, true);
        break;
    }
    case Menu::warninglevellist:
        graphics.bigprint(-1, 45, loc::gettext("WARNING"), tr, tg, tb, true);
        graphics.PrintWrap(-1, 65, FILESYSTEM_getLevelDirError(), tr, tg, tb, true, 10, 304);
        break;
    default:
        break;
    }
}

void titlerender(void)
{

    ClearSurface(graphics.backBuffer);

    if (!game.menustart)
    {
        tr = graphics.col_tr;
        tg = graphics.col_tg;
        tb = graphics.col_tb;

        int temp = 50;
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        const char* editionlabel = loc::gettext("MAKE AND PLAY EDITION");
        graphics.Print(264-graphics.len(editionlabel),temp+35,editionlabel,tr, tg, tb);
#endif

        graphics.PrintWrap(5, 175, loc::gettext("[ Press ACTION to Start ]"), tr, tg, tb, true);
        graphics.PrintWrap(5, 195, loc::gettext("ACTION = Space, Z, or V"), int(tr*0.5f), int(tg*0.5f), int(tb*0.5f), true);
    }
    else
    {
        if(!game.colourblindmode) graphics.drawtowerbackground(graphics.titlebg);

        tr = graphics.col_tr;
        tg = graphics.col_tg;
        tb = graphics.col_tb;

        menurender();

        tr = int(tr * .8f);
        tg = int(tg * .8f);
        tb = int(tb * .8f);
        if (tr < 0) tr = 0;
        if(tr>255) tr=255;
        if (tg < 0) tg = 0;
        if(tg>255) tg=255;
        if (tb < 0) tb = 0;
        if(tb>255) tb=255;
        graphics.drawmenu(tr, tg, tb, game.currentmenuname == Menu::levellist);
    }

    graphics.drawfade();

    graphics.renderwithscreeneffects();
}

void gamecompleterender(void)
{
    ClearSurface(graphics.backBuffer);

    if(!game.colourblindmode) graphics.drawtowerbackground(graphics.titlebg);

    tr = graphics.col_tr;
    tg = graphics.col_tg;
    tb = graphics.col_tb;


    //rendering starts... here!

    int position = graphics.lerp(game.oldcreditposition, game.creditposition);
    if (graphics.onscreen(220 + position))
    {
        int temp = 220 + position;
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
    }

    // TODO LOC. Maybe some of the text needs to be made a _bit_ smaller, or it's too tight (even "Thanks for playing!"). Maybe indicate in translation file "GitHub Contributors" doesn't _really_ need "GitHub" or could be replaced with "Code", and maybe auto-wrap "Thanks for playing"
    if (graphics.onscreen(290 + position)) graphics.bigprint( -1, 290 + position, "Starring", tr, tg, tb, true, 2);

    if (graphics.onscreen(320 + position))
    {
        graphics.drawcrewman(70, 320 + position, 0, true);
        graphics.Print(100, 330 + position, "Captain Viridian", tr, tg, tb);
    }
    if (graphics.onscreen(350 + position))
    {
        graphics.drawcrewman(70, 350 + position, 1, true);
        graphics.Print(100, 360 + position, "Doctor Violet", tr, tg, tb);
    }
    if (graphics.onscreen(380 + position))
    {
        graphics.drawcrewman(70, 380 + position, 2, true);
        graphics.Print(100, 390 + position, "Professor Vitellary", tr, tg, tb);
    }
    if (graphics.onscreen(410 + position))
    {
        graphics.drawcrewman(70, 410 + position, 3, true);
        graphics.Print(100, 420 + position, "Officer Vermilion", tr, tg, tb);
    }
    if (graphics.onscreen(440 + position))
    {
        graphics.drawcrewman(70, 440 + position, 4, true);
        graphics.Print(100, 450 + position, "Chief Verdigris", tr, tg, tb);
    }
    if (graphics.onscreen(470 + position))
    {
        graphics.drawcrewman(70, 470 + position, 5, true);
        graphics.Print(100, 480 + position, "Doctor Victoria", tr, tg, tb);
    }

    if (graphics.onscreen(520 + position)) graphics.bigprint( -1, 520 + position, "Credits", tr, tg, tb, true, 3);

    if (graphics.onscreen(560 + position))
    {
        graphics.Print(40, 560 + position, "Created by", tr, tg, tb);
        graphics.bigprint(60, 570 + position, "Terry Cavanagh", tr, tg, tb);
    }

    if (graphics.onscreen(600 + position))
    {
        graphics.Print(40, 600 + position, "With Music by", tr, tg, tb);
        graphics.bigprint(60, 610 + position, "Magnus P~lsson", tr, tg, tb);
    }

    if (graphics.onscreen(640 + position))
    {
        graphics.Print(40, 640 + position, "Rooms Named by", tr, tg, tb);
        graphics.bigprint(60, 650 + position, "Bennett Foddy", tr, tg, tb);
    }

    if (graphics.onscreen(710 + position))
    {
        graphics.Print(40, 680 + position, "C++ Port by", tr, tg, tb);
        graphics.bigprint(60, 690 + position, "Simon Roth", tr, tg, tb);
        graphics.bigprint(60, 710 + position, "Ethan Lee", tr, tg, tb);
        graphics.bigprint(60, 730 + position, "Misa Kai", tr, tg, tb);
    }


    if (graphics.onscreen(770 + position))
    {
        graphics.Print(40, 760 + position, "Beta Testing by", tr, tg, tb);
        graphics.bigprint(60, 770 + position, "Sam Kaplan", tr, tg, tb);
        graphics.bigprint(60, 790 + position, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(820 + position))
    {
        graphics.Print(40, 820 + position, "Ending Picture by", tr, tg, tb);
        graphics.bigprint(60, 830 + position, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(910 + position)) graphics.bigprint( -1, 890 + position, "Patrons", tr, tg, tb, true, 3);

    int creditOffset = 950;

    for (size_t i = 0; i < SDL_arraysize(Credits::superpatrons); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            graphics.Print(-1, creditOffset + position, Credits::superpatrons[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 10;
    if (graphics.onscreen(creditOffset + position)) graphics.Print( -1, creditOffset + position, "and", tr, tg, tb, true);
    creditOffset += 20;

    for (size_t i = 0; i < SDL_arraysize(Credits::patrons); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            graphics.Print(-1, creditOffset + position, Credits::patrons[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 20;
    if (graphics.onscreen(creditOffset + position)) graphics.bigprint(40, creditOffset + position, "GitHub Contributors", tr, tg, tb, true);
    creditOffset += 30;

    for (size_t i = 0; i < SDL_arraysize(Credits::githubfriends); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            graphics.Print(-1, creditOffset + position, Credits::githubfriends[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 140;
    if (graphics.onscreen(creditOffset + position)) graphics.bigprint( -1, creditOffset + position, "Thanks for playing!", tr, tg, tb, true, 2);

    graphics.drawfade();

    graphics.render();
}

void gamecompleterender2(void)
{
    ClearSurface(graphics.backBuffer);

    graphics.drawimage(10, 0, 0);

    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            if (j == game.creditposy)
            {
                if (i > game.creditposx)
                {
                    FillRect(graphics.backBuffer, i * 8, j * 8, 8, 8, 0, 0, 0);
                }
            }

            if (j > game.creditposy)
            {
                FillRect(graphics.backBuffer, i * 8, j * 8, 8, 8, 0, 0, 0);
            }
        }
    }

    FillRect(graphics.backBuffer, graphics.lerp(game.oldcreditposx * 8, game.creditposx * 8) + 8, game.creditposy * 8, 8, 8, 0, 0, 0);

    graphics.drawfade();

    graphics.render();
}

static const char* interact_prompt(
    char* buffer,
    const size_t buffer_size,
    const char* raw
) {
    const char* string_fmt_loc = SDL_strstr(raw, "%s");
    const char* button;

    if (string_fmt_loc == NULL /* No "%s". */
    || string_fmt_loc != SDL_strchr(raw, '%') /* First "%" found is not "%s". */
    || SDL_strchr(&string_fmt_loc[1], '%') != NULL) /* Other "%" after "%s". */
    {
        return raw;
    }

    if (game.separate_interact)
    {
        button = loc::gettext("E");
    }
    else
    {
        button = loc::gettext("ENTER");
    }

    SDL_snprintf(buffer, buffer_size, raw, button);

    return buffer;
}

void gamerender(void)
{



    if(!game.blackout)
    {

        if (map.towermode)
        {
            if (!game.colourblindmode)
            {
                graphics.drawtowerbackground(graphics.towerbg);
            }
            else
            {
                ClearSurface(graphics.backBuffer);
            }
            graphics.drawtowermap();
        }
        else
        {
            if(!game.colourblindmode)
            {
                graphics.drawbackground(map.background);
            }
            else
            {
                ClearSurface(graphics.backBuffer);
            }
            if ((map.finalmode || map.custommode) && map.final_colormode)
            {
                graphics.drawfinalmap();
            }
            else
            {
                graphics.drawmap();
            }
        }


        graphics.drawentities();
        if (map.towermode)
        {
            graphics.drawtowerspikes();
        }
    }

    if (graphics.fademode==0
    && !game.intimetrial
    && !game.isingamecompletescreen()
    && game.swngame != 1
    && game.showingametimer
    && !roomname_translator::enabled)
    {
        const char* tempstring = loc::gettext("TIME:");
        int label_len = graphics.len(tempstring);
        graphics.bprint(6, 6, tempstring,  255,255,255);
        graphics.bprint(6+label_len, 6, game.timestring(),  196, 196, 196);
    }

    bool force_roomname_hidden = false;
    int roomname_r = 196, roomname_g = 196, roomname_b = 255 - help.glow;
    if (roomname_translator::enabled)
    {
        roomname_translator::overlay_render(
            &force_roomname_hidden,
            &roomname_r, &roomname_g, &roomname_b
        );
    }

    if ((map.extrarow==0 || (map.custommode && map.roomname[0] != '\0')) && !force_roomname_hidden)
    {
        const char* translated_roomname;
        if (map.finalmode)
        {
            translated_roomname = loc::gettext_roomname(game.roomx, game.roomy, map.glitchname, map.roomname_special);
        }
        else
        {
            translated_roomname = loc::gettext_roomname(game.roomx, game.roomy, map.roomname, map.roomname_special);
        }

        graphics.render_roomname(translated_roomname, roomname_r, roomname_g, roomname_b);
    }

    if (map.roomtexton)
    {
        //Draw room text!
        for (size_t i = 0; i < map.roomtext.size(); i++)
        {
            graphics.Print(map.roomtext[i].x*8, (map.roomtext[i].y*8), map.roomtext[i].text, 196, 196, 255 - help.glow);
        }
    }

#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
     if(map.custommode && !map.custommodeforreal && !game.advancetext){
        //Return to level editor
        int alpha = graphics.lerp(ed.oldreturneditoralpha, ed.returneditoralpha);
        graphics.bprintalpha(5, 5, loc::gettext("[Press ENTER to return to editor]"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), alpha, false);
      }
#endif


    graphics.cutscenebars();
    graphics.drawfade();
    BlitSurfaceStandard(graphics.backBuffer, NULL, graphics.tempBuffer, NULL);

    graphics.drawgui();
    if (graphics.flipmode)
    {
        if (game.advancetext) graphics.bprint(5, 228, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }
    else
    {
        if (game.advancetext) graphics.bprint(5, 5, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }

    if (game.readytotele > 100 || game.oldreadytotele > 100)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            loc::gettext("- Press %s to Teleport -")
        );
        int alpha = graphics.lerp(game.oldreadytotele, game.readytotele);

        graphics.bprint(5, graphics.flipmode ? 20 : 210, final_string, alpha - 20 - (help.glow / 2), alpha - 20 - (help.glow / 2), alpha, true);
    }

    if (game.swnmode)
    {
        if (game.swngame == 0)
        {
            std::string tempstring = help.timestring(game.swntimer);
            graphics.bigbprint( -1, 20, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
        }
        else if (game.swngame == 1)
        {
            if (game.swnmessage == 0)
            {
                std::string tempstring = help.timestring(game.swntimer);
                graphics.bprint( 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                const char* besttimelabel = loc::gettext("Best Time");
                graphics.bprint( 320-graphics.len(besttimelabel)-8, 10, besttimelabel, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);

                switch(game.swnbestrank)
                {
                case 0:
                    graphics.PrintWrap( -1, 204, loc::gettext("Next Trophy at 5 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 1:
                    graphics.PrintWrap( -1, 204, loc::gettext("Next Trophy at 10 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 2:
                    graphics.PrintWrap( -1, 204, loc::gettext("Next Trophy at 15 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 3:
                    graphics.PrintWrap( -1, 204, loc::gettext("Next Trophy at 20 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 4:
                    graphics.PrintWrap( -1, 204, loc::gettext("Next Trophy at 30 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 5:
                    graphics.PrintWrap( -1, 204, loc::gettext("Next Trophy at 1 minute"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 6:
                    graphics.Print( -1, 204, loc::gettext("All Trophies collected!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                }
            }
            else if (game.swnmessage == 1)
            {
                std::string tempstring = help.timestring(game.swntimer);
                graphics.bprint( 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                if (int(game.deathseq / 5) % 2 == 1)
                {
                    const char* besttimelabel = loc::gettext("Best Time");
                    graphics.bprint( 320-graphics.len(besttimelabel)-8, 10, besttimelabel, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                    graphics.bigbrprint( 300, 24, tempstring, 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), false, 2);

                    graphics.bigbprint( -1, 200, loc::gettext("New Record!"), 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), true, 2);
                }
            }
            else if (game.swnmessage >= 2)
            {
                game.swnmessage--;
                if (game.swnmessage == 2) game.swnmessage = 0;
                std::string tempstring = help.timestring(game.swntimer);
                graphics.bprint( 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                const char* besttimelabel = loc::gettext("Best Time");
                graphics.bprint( 320-graphics.len(besttimelabel)-8, 10, besttimelabel, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);

                if (int(game.swnmessage / 5) % 2 == 1)
                {
                    graphics.bigbprint( -1, 200, loc::gettext("New Trophy!"), 220 - (help.glow), 128 - (help.glow), 128 - (help.glow / 2), true, 2);
                }
            }

            graphics.bprint( 20, 228, loc::gettext("[Press ENTER to stop]"), 160 - (help.glow/2), 160 - (help.glow/2), 160 - (help.glow/2), true);
        }
        else if(game.swngame==2)
        {
            if (int(game.swndelay / 15) % 2 == 1 || game.swndelay >= 120)
            {
                int y1;
                int y2;
                if (graphics.flipmode)
                {
                    y1 = 30;
                    y2 = 10;
                }
                else
                {
                    y1 = 10;
                    y2 = 30;
                }
                // TODO LOC: wrapped bigprint.
                graphics.bigbprint( -1, y1, "Survive for", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                graphics.bigbprint( -1, y2, "60 seconds!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
            }
        }
        else if(game.swngame==7)
        {
            if (game.swndelay >= 60)
            {
                graphics.bigbprint( -1, 20, loc::gettext("SUPER GRAVITRON"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.bprint( 240, 190, loc::gettext("Best Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                graphics.bigbrprint( 300, 205, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
            }
            else    if (int(game.swndelay / 10) % 2 == 1)
            {
                graphics.bigbprint( -1, 20, loc::gettext("SUPER GRAVITRON"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                graphics.bigbprint( -1, 200, loc::gettext("GO!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 3);
            }
        }
    }

    if (game.intimetrial && graphics.fademode==0)
    {
        //Draw countdown!
        if (game.timetrialcountdown > 0)
        {
            if (game.timetrialcountdown < 30)
            {
                if (int(game.timetrialcountdown / 4) % 2 == 0) graphics.bigbprint( -1, 100, loc::gettext("Go!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 60)
            {
                graphics.bigbprint( -1, 100, "1", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 90)
            {
                graphics.bigbprint( -1, 100, "2", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 120)
            {
                graphics.bigbprint( -1, 100, "3", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
        }
        else if (!roomname_translator::is_pausing())
        {
            //Draw OSD stuff
            const char* tempstring = loc::gettext("TIME:");
            int label_len = graphics.len(tempstring);
            graphics.bprint(6, 18, tempstring,  255,255,255);
            tempstring = loc::gettext("DEATH:");
            label_len = VVV_max(label_len, graphics.len(tempstring));
            graphics.bprint(6, 30, tempstring,  255, 255, 255);
            tempstring = loc::gettext("SHINY:");
            label_len = VVV_max(label_len, graphics.len(tempstring));
            graphics.bprint(6, 42, tempstring,  255,255,255);

            if(game.timetrialparlost)
            {
                graphics.bprint(8+label_len, 18, game.timestring(),  196, 80, 80);
            }
            else
            {
                graphics.bprint(8+label_len, 18, game.timestring(),  196, 196, 196);
            }
            if(game.deathcounts>0)
            {
                graphics.bprint(8+label_len, 30,help.String(game.deathcounts),  196, 80, 80);
            }
            else
            {
                graphics.bprint(8+label_len, 30,help.String(game.deathcounts),  196, 196, 196);
            }
            char buffer[SCREEN_WIDTH_CHARS + 1];
            SDL_snprintf(
                buffer, sizeof(buffer),
                loc::gettext("%d of %d"),
                game.trinkets(), game.timetrialshinytarget
            );
            if(game.trinkets()<game.timetrialshinytarget)
            {
                graphics.bprint(8+label_len, 42, buffer,  196, 80, 80);
            }
            else
            {
                graphics.bprint(8+label_len, 42, buffer,  196, 196, 196);
            }

            tempstring = loc::gettext("PAR TIME:");
            label_len = graphics.len(tempstring)+8;
            if(game.timetrialparlost)
            {
                graphics.bprint(275-label_len, 214, tempstring,  80, 80, 80);
                graphics.bprint(275, 214, game.partimestring(),  80, 80, 80);
            }
            else
            {
                graphics.bprint(275-label_len, 214, tempstring,  255, 255, 255);
                graphics.bprint(275, 214, game.partimestring(),  196, 196, 196);
            }
        }
    }

    float act_alpha = graphics.lerp(game.prev_act_fade, game.act_fade) / 10.0f;
    if(game.act_fade>5 || game.prev_act_fade>5)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            game.activity_lastprompt.c_str()
        );

        int centered_x = ((160 ) - ((graphics.len(final_string)) / 2));

        if (game.activity_r == 0 && game.activity_g == 0 && game.activity_b == 0)
        {
            graphics.bprint(centered_x + game.activity_x, game.activity_y + 12, final_string, 196*act_alpha, 196*act_alpha, (255 - help.glow)*act_alpha);
        }
        else
        {
            graphics.drawtextbox(game.activity_x + 16, game.activity_y + 4, 36, 3, game.activity_r*act_alpha, game.activity_g*act_alpha, game.activity_b*act_alpha);
            graphics.Print(centered_x + game.activity_x, game.activity_y + 12, final_string, game.activity_r*act_alpha, game.activity_g*act_alpha, game.activity_b*act_alpha);
        }
    }

    if (obj.trophytext > 0 || obj.oldtrophytext > 0)
    {
        graphics.drawtrophytext();
    }


    graphics.renderwithscreeneffects();
}

/* Used to keep some graphics positions on the map screen
 * the same in Flip Mode. */
#define FLIP(y, h) (graphics.flipmode ? 220 - (y) - (h) : (y))

void maprender(void)
{
    ClearSurface(graphics.backBuffer);

    //draw screen alliteration
    //Roomname:
    const char* translated_roomname;
    if (map.hiddenname[0] != '\0')
    {
        translated_roomname = loc::gettext_roomname_special(map.hiddenname);
    }
    else
    {
        if (map.finalmode)
        {
            translated_roomname = loc::gettext_roomname(game.roomx, game.roomy, map.glitchname, map.roomname_special);
        }
        else
        {
            translated_roomname = loc::gettext_roomname(game.roomx, game.roomy, map.roomname, map.roomname_special);
        }
    }
    graphics.Print(5, 2, translated_roomname, 196, 196, 255 - help.glow, true);

    //Background color
    FillRect(graphics.backBuffer,0, 12, 320, 240, 10, 24, 26 );



    //Menubar:
    graphics.drawtextbox( -10, 212, 43, 3, 65, 185, 207);

    // Draw the selected page name at the bottom
    // menupage 0 - 3 is the pause screen
    if (script.running && game.menupage == 3)
    {
        // While in a cutscene, you can only save
        graphics.Print(-1, 220, "[" + std::string(loc::gettext("SAVE")) + "]", 196, 196, 255 - help.glow, true);
    }
    else if (game.menupage <= 3)
    {
        const char* tab1;
        if (game.insecretlab)
        {
            tab1 = loc::gettext("GRAV");
        }
        else if (obj.flags[67] && !map.custommode)
        {
            tab1 = loc::gettext("SHIP");
        }
        else
        {
            tab1 = loc::gettext("CREW");
        }
#define TAB(opt, text) graphics.map_tab(opt, text, game.menupage == opt)
        TAB(0, loc::gettext("MAP"));
        TAB(1, tab1);
        TAB(2, loc::gettext("STATS"));
        TAB(3, loc::gettext("SAVE"));
#undef TAB
    }

    // Draw menu header
    switch (game.menupage)
    {
    case 30:
    case 31:
    case 32:
    case 33:
        graphics.Print(-1, 220, loc::gettext("[ PAUSE ]"), 196, 196, 255 - help.glow, true);
    }

    // Draw menu options
    if (game.menupage >= 30 && game.menupage <= 33)
    {
#define OPTION(opt, text) graphics.map_option(opt, 4, text, game.menupage - 30 == opt)
        OPTION(0, loc::gettext("return to game"));
        OPTION(1, loc::gettext("options"));
        OPTION(2, loc::gettext("quit to menu"));
#undef OPTION
    }

    // Draw the actual menu
    switch(game.menupage)
    {
    case 0:
        if (map.finalmode || (map.custommode&&!map.customshowmm))
        {
            //draw the map image
            graphics.drawpixeltextbox(35, 16, 250, 190, 65, 185, 207);
            graphics.drawimage(1, 40, 21, false);
            for (int j = 0; j < 20; j++)
            {
                for (int i = 0; i < 20; i++)
                {
                    graphics.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
                }
            }
            graphics.Print(-1, 105, loc::gettext("NO SIGNAL"), 245, 245, 245, true);
        }
#ifndef NO_CUSTOM_LEVELS
        else if(map.custommode)
        {
          //draw the map image
          graphics.drawpixeltextbox(35+map.custommmxoff, 16+map.custommmyoff, map.custommmxsize+10, map.custommmysize+10, 65, 185, 207);
          if (graphics.minimap_mounted)
          {
            graphics.drawpartimage(1, 40+map.custommmxoff, 21+map.custommmyoff, map.custommmxsize, map.custommmysize);
          }
          else
          {
            graphics.drawpartimage(12, 40+map.custommmxoff, 21+map.custommmyoff, map.custommmxsize,map.custommmysize);
          }

          //Black out here
          if(map.customzoom==4){
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(!map.isexplored(i, j)){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + 9 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + 9+ (j * 36), false);

                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+ 21 + 9 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + 9+ (j * 36), false);

                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + 9 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + 9+ (j * 36)+18, false);

                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + 9 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + 9+ (j * 36)+18, false);
                }
              }
            }
          }else if(map.customzoom==2){
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(!map.isexplored(i, j)){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 24), map.custommmyoff+21 + (j * 18), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 24), map.custommmyoff+21 + (j * 18), false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 24), map.custommmyoff+21 + 9 + (j * 18), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 24), map.custommmyoff+21 + 9+ (j * 18), false);
                }
              }
            }
          }else{
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(!map.isexplored(i, j)){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 12), map.custommmyoff+21 + (j * 9), false);
                }
              }
            }
          }

          //normal size maps
          if(map.customzoom==4){
            if(map.cursorstate==1){
              if (int(map.cursordelay / 4) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 48) +map.custommmxoff, 21 + ((game.roomy - 100) * 36)+map.custommmyoff , 48 , 36 , 255,255,255);
                graphics.drawrect(40 + ((game.roomx - 100) * 48) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 36) + 2+map.custommmyoff, 48 - 4, 36 - 4, 255,255,255);
              }
            }else if (map.cursorstate == 2){
              if (int(map.cursordelay / 15) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 48) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 36) + 2+map.custommmyoff, 48 - 4, 36 - 4, 16, 245 - (help.glow), 245 - (help.glow));
              }
            }
          }else if(map.customzoom==2){
            if(map.cursorstate==1){
              if (int(map.cursordelay / 4) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 24)+map.custommmxoff , 21 + ((game.roomy - 100) * 18)+map.custommmyoff , 24 , 18 , 255,255,255);
                graphics.drawrect(40 + ((game.roomx - 100) * 24) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 18) + 2+map.custommmyoff, 24 - 4, 18 - 4, 255,255,255);
              }
            }else if (map.cursorstate == 2){
              if (int(map.cursordelay / 15) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 24) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 18) + 2+map.custommmyoff, 24 - 4, 18 - 4, 16, 245 - (help.glow), 245 - (help.glow));
              }
            }
          }else{
            if(map.cursorstate==1){
              if (int(map.cursordelay / 4) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 12)+map.custommmxoff , 21 + ((game.roomy - 100) * 9)+map.custommmyoff , 12 , 9 , 255,255,255);
                graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 9) + 2+map.custommmyoff, 12 - 4, 9 - 4, 255,255,255);
              }
            }else if (map.cursorstate == 2){
              if (int(map.cursordelay / 15) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 9) + 2+map.custommmyoff, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));
              }
            }
          }

          if(map.showtrinkets){
            for(size_t i=0; i<map.shinytrinkets.size(); i++){
              if(!obj.collect[i]){
                int temp = 1086;
                if(graphics.flipmode) temp += 3;
                if(map.customzoom==4){
                  graphics.drawtile(40 + (map.shinytrinkets[i].x * 48) + 20+map.custommmxoff, 21 + (map.shinytrinkets[i].y * 36) + 14+map.custommmyoff, temp);
                }else if(map.customzoom==2){
                  graphics.drawtile(40 + (map.shinytrinkets[i].x * 24) + 8+map.custommmxoff, 21 + (map.shinytrinkets[i].y * 18) + 5+map.custommmyoff, temp);
                }else{
                  graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12) + map.custommmxoff, 22 + (map.shinytrinkets[i].y * 9) + map.custommmyoff, temp);
                }
              }
            }
          }
        }
#endif /* NO_CUSTOM_LEVELS */
        else
        {
            //draw the map image
            graphics.drawpixeltextbox(35, 16, 250, 190, 65, 185, 207);
            graphics.drawimage(1, 40, 21, false);

            //black out areas we can't see yet
            for (int j = 0; j < 20; j++)
            {
                for (int i = 0; i < 20; i++)
                {
                    if(!map.isexplored(i, j))
                    {
                        //Draw the fog of war on the map
                        graphics.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
                    }
                }
            }
            //draw the coordinates
            if (game.roomx == 109)
            {
                //tower!instead of room y, scale map.ypos
                if (map.cursorstate == 1)
                {
                    if (int(map.cursordelay / 4) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) , 21 , 12, 180, 255,255,255);
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4, 255,255,255);
                    }
                    if (map.cursordelay > 30) map.cursorstate = 2;
                }
                else if (map.cursorstate == 2)
                {
                    if (int(map.cursordelay / 15) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4,16, 245 - (help.glow), 245 - (help.glow));
                    }
                }
            }
            else
            {
                if (map.cursorstate == 1)
                {
                    if (int(map.cursordelay / 4) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) , 21 + ((game.roomy - 100) * 9) , 12 , 9 , 255,255,255);
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 255,255,255);
                    }
                }
                else if (map.cursorstate == 2)
                {
                    if (int(map.cursordelay / 15) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));
                    }
                }
            }

            //draw legend details
            for (size_t i = 0; i < map.teleporters.size(); i++)
            {
                if (map.showteleporters && map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
                {
                    int temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
                    if (graphics.flipmode) temp += 3;
                    graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
                }
                else if(map.showtargets && !map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
                {
                    int temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
                    if (graphics.flipmode) temp += 3;
                    graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
                }
            }

            if (map.showtrinkets)
            {
                for (size_t i = 0; i < map.shinytrinkets.size(); i++)
                {
                    if (!obj.collect[i])
                    {
                        int temp = 1086;
                        if (graphics.flipmode) temp += 3;
                        graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),    temp);
                    }
                }
            }
        }
        break;
    case 1:
        if (game.insecretlab)
        {
            if (graphics.flipmode)
            {
                graphics.PrintWrap(0, 174, loc::gettext("SUPER GRAVITRON HIGHSCORE"), 196, 196, 255 - help.glow, true);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.Print( 240, 124, loc::gettext("Best Time"), 196, 196, 255 - help.glow, true);
                graphics.bigrprint( 300, 94, tempstring, 196, 196, 255 - help.glow, true, 2);

                switch(game.swnbestrank)
                {
                case 0:
                    graphics.PrintWrap( -1, 40, loc::gettext("Next Trophy at 5 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 1:
                    graphics.PrintWrap( -1, 40, loc::gettext("Next Trophy at 10 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 2:
                    graphics.PrintWrap( -1, 40, loc::gettext("Next Trophy at 15 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 3:
                    graphics.PrintWrap( -1, 40, loc::gettext("Next Trophy at 20 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 4:
                    graphics.PrintWrap( -1, 40, loc::gettext("Next Trophy at 30 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 5:
                    graphics.PrintWrap( -1, 40, loc::gettext("Next Trophy at 1 minute"), 196, 196, 255 - help.glow, true);
                    break;
                case 6:
                    graphics.PrintWrap( -1, 40, loc::gettext("All Trophies collected!"), 196, 196, 255 - help.glow, true);
                    break;
                }
            }
            else
            {
                graphics.PrintWrap(0, 40, loc::gettext("SUPER GRAVITRON HIGHSCORE"), 196, 196, 255 - help.glow, true);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.Print( 240, 90, "Best Time", 196, 196, 255 - help.glow, true);
                graphics.bigrprint( 300, 104, tempstring, 196, 196, 255 - help.glow, true, 2);

                switch(game.swnbestrank)
                {
                case 0:
                    graphics.PrintWrap( -1, 174, loc::gettext("Next Trophy at 5 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 1:
                    graphics.PrintWrap( -1, 174, loc::gettext("Next Trophy at 10 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 2:
                    graphics.PrintWrap( -1, 174, loc::gettext("Next Trophy at 15 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 3:
                    graphics.PrintWrap( -1, 174, loc::gettext("Next Trophy at 20 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 4:
                    graphics.PrintWrap( -1, 174, loc::gettext("Next Trophy at 30 seconds"), 196, 196, 255 - help.glow, true);
                    break;
                case 5:
                    graphics.PrintWrap( -1, 174, loc::gettext("Next Trophy at 1 minute"), 196, 196, 255 - help.glow, true);
                    break;
                case 6:
                    graphics.PrintWrap( -1, 174, loc::gettext("All Trophies collected!"), 196, 196, 255 - help.glow, true);
                    break;
                }
            }
        }
        else if (obj.flags[67] && !map.custommode)
        {
            graphics.PrintWrap(0, 105, loc::gettext("Press ACTION to warp to the ship."), 196, 196, 255 - help.glow, true);
        }
#if !defined(NO_CUSTOM_LEVELS)
        else if(map.custommode){
            LevelMetaData& meta = cl.ListOfMetaData[game.playcustomlevel];

            graphics.bigprint(-1, FLIP(45, 8), meta.title, 196, 196, 255 - help.glow, true);
            char buffer[SCREEN_WIDTH_CHARS + 1];
            SDL_snprintf(
                buffer, sizeof(buffer),
                loc::gettext("by %s"),
                meta.creator.c_str()
            );
            graphics.Print(-1, FLIP(70, 8), buffer, 196, 196, 255 - help.glow, true);
            graphics.Print(-1, FLIP(80, 8), meta.website, 196, 196, 255 - help.glow, true);
            graphics.Print(-1, FLIP(100, 8), meta.Desc1, 196, 196, 255 - help.glow, true);
            graphics.Print(-1, FLIP(110, 8), meta.Desc2, 196, 196, 255 - help.glow, true);
            graphics.Print(-1, FLIP(120, 8), meta.Desc3, 196, 196, 255 - help.glow, true);

            int remaining = cl.numcrewmates() - game.crewmates();

            loc::gettext_plural_fill(buffer, sizeof(buffer), "%s crewmates remain", "%s crewmate remains", remaining);
            graphics.Print(1, FLIP(165, 8), buffer, 196, 196, 255 - help.glow, true);
        }
#endif
        else
        {
            if (graphics.flipmode)
            {
                for (int i = 0; i < 3; i++)
                {
                    graphics.drawcrewman(16, 32 + (i * 64), 2-i, game.crewstats[2-i]);
                    if (game.crewstats[(2-i)])
                    {
                        graphics.printcrewname(44, 32 + (i * 64)+4+10, 2-i);
                        graphics.printcrewnamestatus(44, 32 + (i * 64)+4, 2-i);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44, 32 + (i * 64)+4+10, 2-i);
                        graphics.Print(44, 32 + (i * 64) + 4, loc::gettext("Missing..."), 64,64,64);
                    }

                    graphics.drawcrewman(16+160, 32 + (i * 64), (2-i)+3, game.crewstats[(2-i)+3]);
                    if (game.crewstats[(2-i)+3])
                    {
                        graphics.printcrewname(44+160, 32 + (i * 64)+4+10, (2-i)+3);
                        graphics.printcrewnamestatus(44+160, 32 + (i * 64)+4, (2-i)+3);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44+160, 32 + (i * 64)+4+10, (2-i)+3);
                        graphics.Print(44+160, 32 + (i * 64) + 4, loc::gettext("Missing..."), 64,64,64);
                    }
                }
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    graphics.drawcrewman(16, 32 + (i * 64), i, game.crewstats[i]);
                    if (game.crewstats[i])
                    {
                        graphics.printcrewname(44, 32 + (i * 64)+4, i);
                        graphics.printcrewnamestatus(44, 32 + (i * 64)+4+10, i);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44, 32 + (i * 64)+4, i);
                        graphics.Print(44, 32 + (i * 64) + 4 + 10, loc::gettext("Missing..."), 64,64,64);
                    }

                    graphics.drawcrewman(16+160, 32 + (i * 64), i+3, game.crewstats[i+3]);
                    if (game.crewstats[i+3])
                    {
                        graphics.printcrewname(44+160, 32 + (i * 64)+4, i+3);
                        graphics.printcrewnamestatus(44+160, 32 + (i * 64)+4+10, i+3);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44+160, 32 + (i * 64)+4, i+3);
                        graphics.Print(44+160, 32 + (i * 64) + 4 + 10, loc::gettext("Missing..."), 64,64,64);
                    }
                }
            }
        }
        break;
    case 2:
    {
        int total;
#ifndef NO_CUSTOM_LEVELS
        if (map.custommode)
        {
            total = cl.numtrinkets();
        }
        else
#endif
        {
            total = 20;
        }

        /* Stats. */
        graphics.Print(0, FLIP(52, 8), loc::gettext("[Trinkets found]"), 196, 196, 255 - help.glow, true);
        graphics.Print(0, FLIP(64, 8), help.number_words(game.trinkets()) + " out of " + help.number_words(total), 96, 96, 96, true); // TODO LOC

        graphics.Print(0, FLIP(102, 8), loc::gettext("[Number of Deaths]"), 196, 196, 255 - help.glow, true);
        graphics.Print(0, FLIP(114, 8), help.String(game.deathcounts), 96, 96, 96, true);

        graphics.Print(0, FLIP(152, 8), loc::gettext("[Time Taken]"), 196, 196, 255 - help.glow, true);
        graphics.Print(0, FLIP(164, 8), game.timestring(), 96, 96, 96, true);
        break;
    }
    case 3:
    {
        if (game.inintermission)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Level Replay"), 146, 146, 180, true);
            break;
        }
        if (game.nodeathmode)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in No Death Mode"), 146, 146, 180, true);
            break;
        }
        if (game.intimetrial)
        {
            graphics.PrintWrap(0, 115, loc::gettext("How'd you get here?"), 146, 146, 180, true);
            break;
        }
        if (game.insecretlab)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Secret Lab"), 146, 146, 180, true);
            break;
        }
        if (game.gamesavefailed)
        {
            graphics.PrintWrap(0, 115, loc::gettext("ERROR: Could not save game!"), 146, 146, 180, true);
            break;
        }

        /* We are not in a special case, so draw the save screen now... */

        if (!map.custommode)
        {
            /* FIXME: The text here should be automatically "balance-wrapped" instead of hardcoding the width. */
            graphics.PrintWrap(0, 174, loc::gettext("(Note: The game is autosaved at every teleporter.)"), 146, 146, 180, true, 12, 240);
        }

        if (!game.gamesaved)
        {
            graphics.PrintWrap(0, 80, loc::gettext("[Press ACTION to save your game]"), 255 - help.glow*2, 255 - help.glow*2, 255 - help.glow, true);

            if (map.custommode || game.quicksummary == "")
            {
                break;
            }

            graphics.Print(0, FLIP(100, 8), loc::gettext("Last Save:"), 164 - help.glow/4, 164 - help.glow/4, 164, true);
            graphics.Print(0, FLIP(110, 8), game.quicksummary, 164 - help.glow/4, 164 - help.glow/4, 164, true);
            break;
        }

        /* We are only still here if the game has been quicksaved... */

        graphics.PrintWrap(0, 36, loc::gettext("Game saved ok!"), 255 - help.glow/2, 255 - help.glow/2, 255 - help.glow/2, true);

        graphics.drawpixeltextbox(17, 65, 286, 90, 65, 185, 207);

        if (map.custommode)
        {
            graphics.Print(0, FLIP(90, 8), game.customleveltitle, 25, 255 - help.glow/2, 255 - help.glow/2, true);
        }
        else
        {
            size_t i;
            graphics.Print(0, FLIP(80, 8), game.savearea, 25, 255 - help.glow/2, 255 - help.glow/2, true);
            for (i = 0; i < SDL_arraysize(game.crewstats); ++i)
            {
                /* Crewmates are annoying. Their height is 21 pixels, but to flip them,
                 * we also have to account for their 2-pixel y-offset (and multiply it by 2). */
                graphics.drawcrewman(169 - 3*42 + i*42, FLIP(95, 21 + 2*2), i, game.crewstats[i], true);
            }
        }

        graphics.Print(59, FLIP(132, 8), game.savetime, 255 - help.glow/2, 255 - help.glow/2, 255 - help.glow/2);
        const std::string& trinketcount = help.number_words(game.savetrinkets);
        graphics.Print(262 - graphics.len(trinketcount), FLIP(132, 8), trinketcount, 255 - help.glow/2, 255 - help.glow/2, 255 - help.glow/2);

        graphics.drawsprite(34, FLIP(126, 17), 50, graphics.col_clock);
        graphics.drawsprite(270, FLIP(126, 17), 22, graphics.col_trinket);
        break;
    }
    case 10:
        graphics.Print(-1, 220, loc::gettext("[ QUIT ]"), 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            if (game.inspecial())
            {
                graphics.PrintWrap(0, 135, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 142, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            graphics.Print(80-16, 88, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 76, loc::gettext("yes, quit to menu"),  96, 96, 96);
        }
        else
        {

            if (game.inspecial())
            {
                graphics.PrintWrap(0, 80, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 76, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            graphics.Print(80-16, 130, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 142, loc::gettext("yes, quit to menu"),  96, 96, 96);

        }
        break;
    case 11:
        graphics.Print(-1, 220, loc::gettext("[ QUIT ]"), 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            if (game.inspecial())
            {
                graphics.PrintWrap(0, 135, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 142, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            graphics.Print(80, 88, loc::gettext("no, keep playing"), 96,96,96);
            graphics.Print(80+32-16, 76, loc::gettext("[ YES, QUIT TO MENU ]"),  196, 196, 255 - help.glow);
        }
        else
        {
            if (game.inspecial())
            {
                graphics.PrintWrap(0, 80, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 76, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            graphics.Print(80, 130, loc::gettext("no, keep playing"), 96,96,96);
            graphics.Print(80+32-16, 142, loc::gettext("[ YES, QUIT TO MENU ]"), 196, 196, 255 - help.glow);
        }
        break;
    case 20:
        graphics.Print(-1, 220, loc::gettext("[ GRAVITRON ]"), 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            graphics.PrintWrap(0, 88, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            graphics.Print(80-16, 142, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 130, loc::gettext("yes, return"),  96, 96, 96);
        }
        else
        {
            graphics.PrintWrap(0, 76, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            graphics.Print(80-16, 130, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 142, loc::gettext("yes, return"),  96, 96, 96);
        }

        break;
    case 21:
        graphics.Print(-1, 220, loc::gettext("[ GRAVITRON ]"), 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            graphics.PrintWrap(0, 88, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            graphics.Print(80, 142, loc::gettext("no, keep playing"), 96, 96, 96);
            graphics.Print(80 + 32-16, 130, loc::gettext("[ YES, RETURN ]"),  196, 196, 255 - help.glow);
        }
        else
        {
            graphics.PrintWrap(0, 76, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            graphics.Print(80, 130, loc::gettext("no, keep playing"), 96, 96, 96);
            graphics.Print(80 + 32-16, 142, loc::gettext("[ YES, RETURN ]"),  196, 196, 255 - help.glow);
        }

    }




    // We need to draw the black screen above the menu in order to disguise it
    // being jankily brought down in glitchrunner mode when exiting to the title
    // Otherwise, there's no reason to obscure the menu
    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2)
    || graphics.fademode == 3
    || graphics.fademode == 5
    || game.fadetomenu
    || game.fadetolab)
    {
        graphics.drawfade();
    }

    if (graphics.resumegamemode || graphics.menuoffset > 0 || graphics.oldmenuoffset > 0)
    {
        graphics.menuoffrender();
    }
    else
    {
        graphics.renderwithscreeneffects();
    }
}

#undef FLIP

void teleporterrender(void)
{
    ClearSurface(graphics.backBuffer);
    int tempx;
    int tempy;
    //draw screen alliteration
    //Roomname:
    const char* translated_roomname;
    int temp = map.area(game.roomx, game.roomy);
    if (temp < 2 && !map.custommode && graphics.fademode==0)
    {
        translated_roomname = loc::gettext_roomname_special(map.hiddenname);
    }
    else
    {
        translated_roomname = loc::gettext_roomname(game.roomx, game.roomy, map.roomname, map.roomname_special);
    }
    graphics.Print(5, 2, translated_roomname, 196, 196, 255 - help.glow, true);

    //Background color
    FillRect(graphics.backBuffer, 0, 12, 320, 240, 10, 24, 26);

    //draw the map image
    graphics.drawpixeltextbox(35, 16, 250, 190, 65, 185, 207);
    graphics.drawimage(1, 40, 21, false);
    //black out areas we can't see yet
    for (int j = 0; j < 20; j++)
    {
        for (int i = 0; i < 20; i++)
        {
            if(!map.isexplored(i, j))
            {
                graphics.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
            }
        }
    }

    //draw the coordinates //current
    if (game.roomx == 109)
    {
        //tower!instead of room y, scale map.ypos
        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21  + 2, 12 - 4, 180 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));
    }
    else
    {
        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));
    }

    if (game.useteleporter)
    {
        //Draw the chosen destination coordinate!
        //TODO
        //draw the coordinates //destination
        int tempx_ = map.teleporters[game.teleport_to_teleporter].x;
        int tempy_ = map.teleporters[game.teleport_to_teleporter].y;
        graphics.drawrect(40 + (tempx_ * 12) + 1, 21 + (tempy_ * 9) + 1, 12 - 2, 9 - 2, 245 - (help.glow * 2), 16, 16);
        graphics.drawrect(40 + (tempx_ * 12) + 3, 21 + (tempy_ * 9) + 3, 12 - 6, 9 - 6, 245 - (help.glow * 2), 16, 16);
    }

    //draw legend details
    for (size_t i = 0; i < map.teleporters.size(); i++)
    {
        if (map.showteleporters && map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
        {
            temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
            if (graphics.flipmode) temp += 3;
            graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
        }
        else if(map.showtargets && !map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
        {
            temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
            if (graphics.flipmode) temp += 3;
            graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
        }
    }

    if (map.showtrinkets)
    {
        for (size_t i = 0; i < map.shinytrinkets.size(); i++)
        {
            if (!obj.collect[i])
            {
                temp = 1086;
                if (graphics.flipmode) temp += 3;
                graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),    temp);
            }
        }
    }

    tempx = map.teleporters[game.teleport_to_teleporter].x;
    tempy = map.teleporters[game.teleport_to_teleporter].y;
    if (game.useteleporter && ((help.slowsine%16)>8))
    {
        //colour in the legend
        temp = 1128;
        if (graphics.flipmode) temp += 3;
        graphics.drawtile(40 + 3 + (tempx * 12), 22 + (tempy * 9), temp);
    }

    graphics.cutscenebars();


    if (game.useteleporter)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            loc::gettext("Press %s to Teleport")
        );

        //Instructions!
        graphics.Print(5, 210, loc::gettext("Press Left/Right to choose a Teleporter"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
        graphics.Print(5, 225, final_string, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }

    graphics.drawgui();

    if (graphics.flipmode)
    {
        if (game.advancetext) graphics.bprint(5, 228, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }
    else
    {
        if (game.advancetext) graphics.bprint(5, 5, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }


    if (graphics.resumegamemode || graphics.menuoffset > 0 || graphics.oldmenuoffset > 0)
    {
        graphics.menuoffrender();
    }
    else
    {
        graphics.render();
    }
}
