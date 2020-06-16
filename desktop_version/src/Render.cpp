#include "Render.h"

#include "Graphics.h"
#include "UtilityClass.h"
#include "Maths.h"
#include "Entity.h"
#include "Map.h"
#include "Script.h"
#include "FileSystemUtils.h"
#include "Localization.h"

#include "MakeAndPlay.h"

int tr;
int tg;
int tb;

void menurender()
{
    int temp = 50;

    switch (game.currentmenuname)
    {
    case Menu::mainmenu:
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        #define EDITIONLABEL loc::gettext("MAKE AND PLAY EDITION")
        graphics.Print(264-graphics.len(EDITIONLABEL),temp+35,EDITIONLABEL,tr, tg, tb);
        #undef EDITIONLABEL
#endif
        graphics.Print( 310 - (4*8), 230, "v2.3", tr/2, tg/2, tb/2);

        if(music.mmmmmm){
            graphics.Print( 10, 230, loc::gettext("[MMMMMM Mod Installed]"), tr/2, tg/2, tb/2);
        }
        break;
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::levellist:
    {
      if(ed.ListOfMetaData.size()==0){
      graphics.PrintWrap( -1, 90, loc::gettext("ERROR: No levels found."), tr, tg, tb, true);
      }
      int tmp=game.currentmenuoption+(game.levelpage*8);
      if(tmp>=0 && tmp < (int) ed.ListOfMetaData.size()){ // FIXME: size_t/int! -flibit
        //Don't show next/previous page or return to menu options here!
        if(game.menuoptions.size() - game.currentmenuoption<=3){

        }else{
          graphics.bigprint( -1, 15, ed.ListOfMetaData[tmp].title, tr, tg, tb, true);
          char creatorline[41]; // ouch, but translators may need to add something after the name
          const std::string& bytext = loc::gettext("by %s").substr(0, 40); // just in case :)
          sprintf(
            creatorline,
            bytext.c_str(),
            ed.ListOfMetaData[tmp].creator.substr(0, 40-(bytext.size()-2)).c_str()
          );
          graphics.Print( -1, 40, creatorline, tr, tg, tb, true);
          graphics.Print( -1, 50, ed.ListOfMetaData[tmp].website, tr, tg, tb, true);
          graphics.Print( -1, 70, ed.ListOfMetaData[tmp].Desc1, tr, tg, tb, true);
          graphics.Print( -1, 80, ed.ListOfMetaData[tmp].Desc2, tr, tg, tb, true);
          graphics.Print( -1, 90, ed.ListOfMetaData[tmp].Desc3, tr, tg, tb, true);
        }
      }
      break;
    }
#endif
    case Menu::errornostart:
      graphics.PrintWrap( -1, 65, loc::gettext("ERROR: This level has no start point!"), tr, tg, tb, true);
      break;
    case Menu::options:
#if defined(MAKEANDPLAY)
#define OFFSET -1
#else
#define OFFSET 0
#endif

        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Accessibility"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Disable screen effects, enable slowdown modes or invincibility"), tr, tg, tb, true);
            break;
#if !defined(MAKEANDPLAY)
        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Unlock Play Modes"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Unlock parts of the game normally unlocked as you progress"), tr, tg, tb, true);
            break;
#endif
        case OFFSET+2:
            graphics.bigprint( -1, 30, loc::gettext("Game Pad Options"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Rebind your controller's buttons and adjust sensitivity"), tr, tg, tb, true);
            break;
        case OFFSET+3:
            graphics.bigprint( -1, 30, loc::gettext("Language"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Change the language"), tr, tg, tb, true);
            break;
        case OFFSET+4:
            graphics.bigprint( -1, 30, loc::gettext("Clear Data"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Delete your save data and unlocked play modes"), tr, tg, tb, true);
            break;
        case OFFSET+5:
            if(music.mmmmmm){
                graphics.bigprint( -1, 30, loc::gettext("Soundtrack"), tr, tg, tb, true);
                graphics.PrintWrap( -1, 65, loc::gettext("Toggle between MMMMMM and PPPPPP"), tr, tg, tb, true);
                if(music.usingmmmmmm){
                    graphics.PrintWrap( -1, 85, loc::gettext("Current soundtrack: MMMMMM"), tr, tg, tb, true);
                }else{
                    graphics.PrintWrap( -1, 85, loc::gettext("Current soundtrack: PPPPPP"), tr, tg, tb, true);
                }
            }
            break;
        }
#undef OFFSET
        break;
    case Menu::graphicoptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Toggle Fullscreen"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Change to fullscreen/windowed mode."), tr, tg, tb, true);

            if(game.fullscreen){
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: FULLSCREEN"), tr, tg, tb, true);
            }else{
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: WINDOWED"), tr, tg, tb, true);
            }
            break;

        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Graphics Mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Choose letterbox/stretch/integer mode."), tr, tg, tb, true);

            if(game.stretchMode == 2){
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: INTEGER"), tr, tg, tb, true);
            }else if (game.stretchMode == 1){
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: STRETCH"), tr, tg, tb, true);
            }else{
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: LETTERBOX"), tr, tg, tb, true);
            }
            break;
        case 2:
            graphics.bigprint( -1, 30, loc::gettext("Toggle Filter"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Change to nearest/linear filter."), tr, tg, tb, true);

            if(game.useLinearFilter){
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: LINEAR"), tr, tg, tb, true);
            }else{
              graphics.PrintWrap( -1, 85, loc::gettext("Current mode: NEAREST"), tr, tg, tb, true);
            }
            break;

        case 3:
            graphics.bigprint( -1, 30, loc::gettext("Analogue Mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("There is nothing wrong with your television set. Do not attempt to adjust the picture."), tr, tg, tb, true);
            break;
        case 4:
            graphics.bigprint(-1, 30, loc::gettext("Toggle Mouse Cursor"), tr, tg, tb, true);
            graphics.PrintWrap(-1, 65, loc::gettext("Show/hide the system mouse cursor."), tr, tg, tb, true);

            if (graphics.showmousecursor) {
                graphics.PrintWrap(-1, 85, loc::gettext("Current mode: SHOW"), tr, tg, tb, true);
            }
            else {
                graphics.PrintWrap(-1, 85, loc::gettext("Current mode: HIDE"), tr/2, tg/2, tb/2, true);
            }
            break;
        }
        break;
    case Menu::credits:
        graphics.Print( -1, 50, loc::gettext("VVVVVV is a game by"), tr, tg, tb, true);
        graphics.bigprint( 40, 65, loc::gettext("Terry Cavanagh"), tr, tg, tb, true, 2);

        graphics.drawimagecol(7, -1, 86, tr *0.75, tg *0.75, tb *0.75, true);

        graphics.Print( -1, 120, loc::gettext("and features music by"), tr, tg, tb, true);
        graphics.bigprint( 40, 135, loc::gettext("Magnus P~lsson"), tr, tg, tb, true, 2);
        graphics.drawimagecol(8, -1, 156, tr *0.75, tg *0.75, tb *0.75, true);
        break;
    case Menu::credits2:
        graphics.Print( -1, 50, loc::gettext("Roomnames are by"), tr, tg, tb, true);
        graphics.bigprint( 40, 65, loc::gettext("Bennett Foddy"), tr, tg, tb, true);
        graphics.drawimagecol(9, -1, 86, tr*0.75, tg *0.75, tb *0.75, true);
        graphics.Print( -1, 110, loc::gettext("C++ version by"), tr, tg, tb, true);
        graphics.bigprint( 40, 125, loc::gettext("Simon Roth"), tr, tg, tb, true);
        graphics.bigprint( 40, 145, loc::gettext("Ethan Lee"), tr, tg, tb, true);
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
        int endidx = std::min(startidx + 9, (int)game.superpatrons.size());

        int xofs = 80 - 16;
        int yofs = 40 + 20;

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(xofs, yofs, game.superpatrons[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits4:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("and also by"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = std::min(startidx + 14, (int)game.patrons.size());

        int maxheight = 10 * 14;
        int totalheight = (endidx - startidx) * 10;
        int emptyspace = maxheight - totalheight;

        int yofs = 40 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(80, yofs, game.patrons[i], tr, tg, tb);
            yofs += 10;
        }
        break;
    }
    case Menu::credits5:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("With contributions on GitHub from"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = std::min(startidx + 9, (int)game.githubfriends.size());

        int maxheight = 14 * 9;
        int totalheight = (endidx - startidx) * 14;
        int emptyspace = maxheight - totalheight;

        int xofs = 80 - 16;
        int yofs = 40 + 20 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(xofs, yofs, game.githubfriends[i], tr, tg, tb);
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
        graphics.bigprint( -1, 40, loc::gettext("Game Speed"), tr, tg, tb, true);
        graphics.PrintWrap( -1, 75, loc::gettext("Select a new game speed below."), tr, tg, tb, true);
        switch (game.gameframerate)
        {
        case 34:
            graphics.PrintWrap( -1, 105, loc::gettext("Game speed is normal."), tr/2, tg/2, tb/2, true);
            break;
        case 41:
            graphics.PrintWrap( -1, 105, loc::gettext("Game speed is at 80%"), tr, tg, tb, true);
            break;
        case 55:
            graphics.PrintWrap( -1, 105, loc::gettext("Game speed is at 60%"), tr, tg, tb, true);
            break;
        case 83:
            graphics.PrintWrap( -1, 105, loc::gettext("Game speed is at 40%"), tr, tg, tb, true);
            break;
        }
        break;
    case Menu::newgamewarning:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure? This will delete your current saves..."), tr, tg, tb, true);
        break;
    case Menu::cleardatamenu:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure you want to delete all your saved data?"), tr, tg, tb, true);
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
            graphics.Print(32, 85, loc::gettext("Low"), tr, tg, tb);
            graphics.Print(-1, 85, loc::gettext("Medium"), tr, tg, tb, true);
            #define HIGHLABEL loc::gettext("High")
            graphics.Print(288-graphics.len(HIGHLABEL), 85, HIGHLABEL, tr, tg, tb);
            #undef HIGHLABEL
            switch(game.controllerSensitivity)
            {
            case 0:
                graphics.Print( -1, 95, "[]..........................", tr, tg, tb, true);
                break;
            case 1:
                graphics.Print( -1, 95, ".......[]...................", tr, tg, tb, true);
                break;
            case 2:
                graphics.Print( -1, 95, ".............[].............", tr, tg, tb, true);
                break;
            case 3:
                graphics.Print( -1, 95, "...................[].......", tr, tg, tb, true);
                break;
            case 4:
                graphics.Print( -1, 95, "..........................[]", tr, tg, tb, true);
                break;
            }
            break;
        case 1:
        case 2:
        case 3:
            graphics.Print( -1, 85, loc::gettext("Flip is bound to: ") + std::string(help.GCString(game.controllerButton_flip)) , tr, tg, tb, true);
            graphics.Print( -1, 95, loc::gettext("Enter is bound to: ")  + std::string(help.GCString(game.controllerButton_map)), tr, tg, tb, true);
            graphics.Print( -1, 105, loc::gettext("Menu is bound to: ") + std::string(help.GCString(game.controllerButton_esc)) , tr, tg, tb, true);
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
        else if (loc::show_lang_maint_menu)
        {
            graphics.PrintWrap(-1, 15, loc::gettext("This menu is only shown when having a lang folder in your VVVVVV folder."), tr, tg, tb, true);
        }
        break;
    case Menu::language_maint:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, loc::gettext("Sync language files"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Merge all new strings from the template files into the translation files, keeping existing translations"), tr, tg, tb, true);
            break;
        case 1:
            graphics.bigprint( -1, 30, loc::gettext("Statistics"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Count the amount of untranslated strings for each language"), tr, tg, tb, true);
            break;
        case 2:
            graphics.bigprint( -1, 30, loc::gettext("Test mode"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 65, loc::gettext("Show all translatable strings as a V or X, revealing untranslatable or forgotten strings. X means missing from file."), tr, tg, tb, true);
        }
        break;
    case Menu::language_maint_sync:
        graphics.PrintWrap(-1, 80, loc::gettext("If new strings were added to the English template language files, this feature will insert them in the translation files for all languages. Only languages that are in the VVVVVV folder can be updated. Make a backup, just in case."), tr, tg, tb, true);
        break;
    case Menu::accessibility:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 40, loc::gettext("Backgrounds"), tr, tg, tb, true);
            if (!game.colourblindmode)
            {
                graphics.PrintWrap( -1, 75, loc::gettext("Backgrounds are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 75, loc::gettext("Backgrounds are OFF."), tr/2, tg/2, tb/2, true);
            }
            break;
        case 1:
            graphics.bigprint( -1, 40, loc::gettext("Screen Effects"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 75, loc::gettext("Disables screen shakes and flashes."), tr, tg, tb, true);
            if (!game.noflashingmode)
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Screen Effects are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Screen Effects are OFF."), tr/2, tg/2, tb/2, true);
            }
            break;
        case 2:
            graphics.bigprint( -1, 40, loc::gettext("Text Outline"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 75, loc::gettext("Disables outline on game text"), tr, tg, tb, true);
            // FIXME: Maybe do an outlined print instead? -flibit
            if (!graphics.notextoutline)
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Text outlines are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 95, loc::gettext("Text outlines are OFF."), tr/2, tg/2, tb/2, true);
            }
            break;
        case 3:
            graphics.bigprint( -1, 40, loc::gettext("Invincibility"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 75, loc::gettext("Provided to help disabled gamers explore the game. Can cause glitches."), tr, tg, tb, true);
            if (map.invincibility)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Invincibility is ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Invincibility is off."), tr/2, tg/2, tb/2, true);
            }
            break;
        case 4:
            graphics.bigprint( -1, 40, loc::gettext("Game Speed"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 75, loc::gettext("May be useful for disabled gamers using one switch devices."), tr, tg, tb, true);
            if (game.gameframerate==34)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Game speed is normal."), tr/2, tg/2, tb/2, true);
            }
            else if (game.gameframerate==41)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Game speed is at 80%"), tr, tg, tb, true);
            }
            else if (game.gameframerate==55)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Game speed is at 60%"), tr, tg, tb, true);
            }
            else if (game.gameframerate==83)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("Game speed is at 40%"), tr, tg, tb, true);
            }
            break;
        case 5:
            graphics.bigprint(-1, 30, loc::gettext("Fake Load Screen"), tr, tg, tb, true);
            if (game.skipfakeload)
                graphics.PrintWrap(-1, 75, loc::gettext("Fake loading screen is OFF"), tr/2, tg/2, tb/2, true);
            else
                graphics.PrintWrap(-1, 75, loc::gettext("Fake loading screen is ON"), tr, tg, tb, true);
            break;
        case 6:
            graphics.bigprint(-1, 30, loc::gettext("Room Name BG"), tr, tg, tb, true);
            graphics.PrintWrap( -1, 75, loc::gettext("Lets you see through what is behind the name at the bottom of the screen."), tr, tg, tb, true);
            if (graphics.translucentroomname)
                graphics.PrintWrap(-1, 105, loc::gettext("Room name background is TRANSLUCENT"), tr/2, tg/2, tb/2, true);
            else
                graphics.PrintWrap(-1, 105, loc::gettext("Room name background is OPAQUE"), tr, tg, tb, true);
            break;
        }
        break;
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

            if (game.gameframerate > 34 || map.invincibility)
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

            if (game.gameframerate > 34 || map.invincibility)
            {
                graphics.PrintWrap( -1, 105, loc::gettext("No Death Mode is not available with slowdown or invincibility."), tr, tg, tb, true);
            }
            else if (!game.unlock[17])
            {
                graphics.PrintWrap( -1, 105, loc::gettext("TO UNLOCK: Achieve an S-rank or above in at least 4 time trials."), tr, tg, tb, true);
            }
            break;
        case 3:
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
        graphics.crewframedelay--;
        if (graphics.crewframedelay <= 0)
        {
            graphics.crewframedelay = 8;
            graphics.crewframe = (graphics.crewframe + 1) % 2;
        }
        switch (game.currentmenuoption)
        {
        case 0:
            //Show teleporter save info
            graphics.drawpixeltextbox(25, 65-20, 270, 90, 34,12, 65, 185, 207,0,4);

            graphics.bigprint(-1, 20, loc::gettext("Tele Save"), tr, tg, tb, true);
            graphics.Print(0, 80-20, game.tele_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true); // TODO LOC. Should translate only at presentation (here), not save to save file, to accomodate language changes. Repeat below
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.tele_crewstats[i], true);
            }
            graphics.Print(160 - 84, 132-20, game.tele_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            graphics.Print(160 + 40, 132-20, help.number(game.tele_trinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.drawspritesetcol(50, 126-20, 50, 18);
            graphics.drawspritesetcol(175, 126-20, 22, 18);
            break;
        case 1:
            //Show quick save info
            graphics.drawpixeltextbox(25, 65-20, 270, 90, 34,12, 65, 185, 207,0,4);

            graphics.bigprint(-1, 20, loc::gettext("Quick Save"), tr, tg, tb, true);
            graphics.Print(0, 80-20, game.quick_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.quick_crewstats[i], true);
            }
            graphics.Print(160 - 84, 132-20, game.quick_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            graphics.Print(160 + 40, 132-20, help.number(game.quick_trinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.drawspritesetcol(50, 126-20, 50, 18);
            graphics.drawspritesetcol(175, 126-20, 22, 18);
            break;
        }
        break;
    case Menu::gameover:
    case Menu::gameover2:
    {
        graphics.bigprint( -1, 25, loc::gettext("GAME OVER"), tr, tg, tb, true, 3);

        graphics.crewframedelay--;
        if (graphics.crewframedelay <= 0)
        {
            graphics.crewframedelay = 8;
            graphics.crewframe = (graphics.crewframe + 1) % 2;
        }
        for (int i = 0; i < 6; i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.crewstats[i], true);
        }
        std::string tempstring; // TODO LOC, plural forms
        tempstring = "You rescued " + help.number(game.crewrescued()) + " crewmates";
        graphics.Print(0, 100, tempstring, tr, tg, tb, true);

        tempstring = "and found " + help.number(game.trinkets()) + " trinkets.";
        graphics.Print(0, 110, tempstring, tr, tg, tb, true);

        graphics.Print(0, 145, loc::gettext("You managed to reach:"), tr, tg, tb, true);
        graphics.Print(0, 155, game.hardestroom, tr, tg, tb, true);

        switch (game.crewrescued())
        {
        case 1:
            tempstring = loc::gettext("Keep trying! You'll get there!");
            break;
        case 2:
            tempstring = loc::gettext("Nice one!");
            break;
        case 3:
            tempstring = loc::gettext("Wow! Congratulations!");
            break;
        case 4:
            tempstring = loc::gettext("Incredible!");
            break;
        case 5:
            tempstring = loc::gettext("Unbelievable! Well done!");
            break;
        case 6:
            tempstring = loc::gettext("Er, how did you do that?");
            break;
        }

        graphics.PrintWrap(0, 190, tempstring, tr, tg, tb, true);
        break;
    }
    case Menu::nodeathmodecomplete:
    case Menu::nodeathmodecomplete2:
    {
        graphics.bigprint( -1, 8, loc::gettext("WOW"), tr, tg, tb, true, 4);

        graphics.crewframedelay--;
        if (graphics.crewframedelay <= 0)
        {
            graphics.crewframedelay = 8;
            graphics.crewframe = (graphics.crewframe + 1) % 2;
        }
        for (int i = 0; i < 6; i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.crewstats[i], true);
        }
        std::string tempstring = loc::gettext("You rescued all the crewmates!");
        graphics.Print(0, 100, tempstring, tr, tg, tb, true);

        tempstring = "And you found " + help.number(game.trinkets()) + " trinkets."; // TODO LOC
        graphics.Print(0, 110, tempstring, tr, tg, tb, true);

        graphics.PrintWrap(0, 160, loc::gettext("A new trophy has been awarded and placed in the secret lab to acknowledge your achievement!"), tr, tg, tb, true);
        break;
    }
    case Menu::timetrialcomplete:
    case Menu::timetrialcomplete2:
    case Menu::timetrialcomplete3:
    {
        graphics.bigprint( -1, 20, loc::gettext("Results"), tr, tg, tb, true, 3);

        std::string tempstring = game.resulttimestring() + " / " + game.partimestring();

        graphics.drawspritesetcol(30, 80-15, 50, 22);
        graphics.Print(65, 80-15, loc::gettext("TIME TAKEN:"), 255, 255, 255);
        graphics.Print(65, 90-15, tempstring, tr, tg, tb);
        if (game.timetrialresulttime <= game.timetrialpar)
        {
            graphics.Print(220, 90-15, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        tempstring = help.String(game.deathcounts);
        graphics.drawspritesetcol(30-4, 80+20-4, 12, 22);
        graphics.Print(65, 80+20, loc::gettext("NUMBER OF DEATHS:"), 255, 255, 255);
        graphics.Print(65, 90+20, tempstring, tr, tg, tb);
        if (game.deathcounts == 0)
        {
            graphics.Print(220, 90+20, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        char tempstring_c[40];
        sprintf(
            tempstring_c,
            loc::gettext("%d of %d").substr(0,39).c_str(),
            game.trinkets(), game.timetrialshinytarget
        );
        graphics.drawspritesetcol(30, 80+55, 22, 22);
        graphics.Print(65, 80+55, loc::gettext("SHINY TRINKETS:"), 255, 255, 255);
        graphics.Print(65, 90+55, tempstring, tr, tg, tb);
        if (game.trinkets() >= game.timetrialshinytarget)
        {
            graphics.Print(220, 90+55, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        int rankw = graphics.len(loc::gettext("Rank:")) + 48;
        int rankx = (320-rankw)/2;
        int rankx2 = rankx + rankw - 32;
        if (game.currentmenuname == Menu::timetrialcomplete2 || game.currentmenuname == Menu::timetrialcomplete3)
        {
            graphics.bigprint( rankx, 175, loc::gettext("Rank:"), tr, tg, tb, false, 2);
        }

        if (game.currentmenuname == Menu::timetrialcomplete3)
        {
            switch(game.timetrialrank)
            {
            case 0:
                graphics.bigprint( rankx2, 165, loc::gettext("B"), 255, 255, 255, false, 4);
                break;
            case 1:
                graphics.bigprint( rankx2, 165, loc::gettext("A"), 255, 255, 255, false, 4);
                break;
            case 2:
                graphics.bigprint( rankx2, 165, loc::gettext("S"), 255, 255, 255, false, 4);
                break;
            case 3:
                graphics.bigprint( rankx2, 165, loc::gettext("V"), 255, 255, 255, false, 4);
                break;
            }
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
        graphics.bigprint( -1, 25, loc::gettext("Unlock Play Modes"), tr, tg, tb, true, 2);

        graphics.PrintWrap( -1, 55, loc::gettext("From here, you may unlock parts of the game that are normally unlocked as you play."), tr, tg, tb, true);
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
    default:
        break;
    }
}

void titlerender()
{

    FillRect(graphics.backBuffer, 0,0,graphics.backBuffer->w, graphics.backBuffer->h, 0x00000000 );

    if (!game.menustart)
    {
        tr = (int)(164 - (help.glow / 2) - int(fRandom() * 4));
        tg = 164 - (help.glow / 2) - int(fRandom() * 4);
        tb = 164 - (help.glow / 2) - int(fRandom() * 4);

        int temp = 50;
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        #define EDITIONLABEL loc::gettext("MAKE AND PLAY EDITION")
        graphics.Print(264-graphics.len(EDITIONLABEL),temp+35,EDITIONLABEL,tr, tg, tb);
        #undef EDITIONLABEL
#endif

        graphics.PrintWrap(5, 175, loc::gettext("[ Press ACTION to Start ]"), tr, tg, tb, true);
        graphics.PrintWrap(5, 195, loc::gettext("ACTION = Space, Z, or V"), int(tr*0.5f), int(tg*0.5f), int(tb*0.5f), true);
    }
    else
    {
        if(!game.colourblindmode) graphics.drawtowerbackground();

        tr = map.r - (help.glow / 4) - int(fRandom() * 4);
        tg = map.g - (help.glow / 4) - int(fRandom() * 4);
        tb = map.b - (help.glow / 4) - int(fRandom() * 4);
        if (tr < 0) tr = 0;
        if(tr>255) tr=255;
        if (tg < 0) tg = 0;
        if(tg>255) tg=255;
        if (tb < 0) tb = 0;
        if(tb>255) tb=255;

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
        if (game.currentmenuname == Menu::timetrials || game.currentmenuname == Menu::unlockmenutrials)
        {
            graphics.drawmenu(tr, tg, tb, 15);
        }
        else if (game.currentmenuname == Menu::unlockmenu)
        {
            graphics.drawmenu(tr, tg, tb, 15);
        }
        else if (game.currentmenuname == Menu::playmodes)
        {
            graphics.drawmenu(tr, tg, tb, 20);
        }
        else if (game.currentmenuname == Menu::mainmenu)
        {
            graphics.drawmenu(tr, tg, tb, 15);
        }
        else if (game.currentmenuname == Menu::playerworlds)
        {
            graphics.drawmenu(tr, tg, tb, 15);
        }
        else if (game.currentmenuname == Menu::levellist)
        {
            graphics.drawlevelmenu(tr, tg, tb, 5);
        }
        else if (game.currentmenuname == Menu::language)
        {
            graphics.drawmenu(tr, tg, tb, 5);
        }
        else
        {
            graphics.drawmenu(tr, tg, tb);
        }
    }

    graphics.drawfade();

    graphics.renderwithscreeneffects();
}

void gamecompleterender()
{
    FillRect(graphics.backBuffer, 0x000000);

    if(!game.colourblindmode) graphics.drawtowerbackground();

    tr = map.r - (help.glow / 4) - fRandom() * 4;
    tg = map.g - (help.glow / 4) - fRandom() * 4;
    tb = map.b - (help.glow / 4) - fRandom() * 4;
    if (tr < 0) tr = 0;
    if(tr>255) tr=255;
    if (tg < 0) tg = 0;
    if(tg>255) tg=255;
    if (tb < 0) tb = 0;
    if(tb>255) tb=255;


    //rendering starts... here!

    if (graphics.onscreen(220 + game.creditposition))
    {
        int temp = 220 + game.creditposition;
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
    }

    // TODO LOC. Maybe some of the text needs to be made a _bit_ smaller, or it's too tight (even "Thanks for playing!"). Maybe indicate in translation file "GitHub Contributors" doesn't _really_ need "GitHub" or could be replaced with "Code", and maybe auto-wrap "Thanks for playing"
    if (graphics.onscreen(290 + game.creditposition)) graphics.bigprint( -1, 290 + game.creditposition, "Starring", tr, tg, tb, true, 2);

    if (graphics.onscreen(320 + game.creditposition))
    {
        graphics.drawcrewman(70, 320 + game.creditposition, 0, true);
        graphics.Print(100, 330 + game.creditposition, "Captain Viridian", tr, tg, tb);
    }
    if (graphics.onscreen(350 + game.creditposition))
    {
        graphics.drawcrewman(70, 350 + game.creditposition, 1, true);
        graphics.Print(100, 360 + game.creditposition, "Doctor Violet", tr, tg, tb);
    }
    if (graphics.onscreen(380 + game.creditposition))
    {
        graphics.drawcrewman(70, 380 + game.creditposition, 2, true);
        graphics.Print(100, 390 + game.creditposition, "Professor Vitellary", tr, tg, tb);
    }
    if (graphics.onscreen(410 + game.creditposition))
    {
        graphics.drawcrewman(70, 410 + game.creditposition, 3, true);
        graphics.Print(100, 420 + game.creditposition, "Officer Vermilion", tr, tg, tb);
    }
    if (graphics.onscreen(440 + game.creditposition))
    {
        graphics.drawcrewman(70, 440 + game.creditposition, 4, true);
        graphics.Print(100, 450 + game.creditposition, "Chief Verdigris", tr, tg, tb);
    }
    if (graphics.onscreen(470 + game.creditposition))
    {
        graphics.drawcrewman(70, 470 + game.creditposition, 5, true);
        graphics.Print(100, 480 + game.creditposition, "Doctor Victoria", tr, tg, tb);
    }

    if (graphics.onscreen(520 + game.creditposition)) graphics.bigprint( -1, 520 + game.creditposition, "Credits", tr, tg, tb, true, 3);

    if (graphics.onscreen(560 + game.creditposition))
    {
        graphics.Print(40, 560 + game.creditposition, "Created by", tr, tg, tb);
        graphics.bigprint(60, 570 + game.creditposition, "Terry Cavanagh", tr, tg, tb);
    }

    if (graphics.onscreen(600 + game.creditposition))
    {
        graphics.Print(40, 600 + game.creditposition, "With Music by", tr, tg, tb);
        graphics.bigprint(60, 610 + game.creditposition, "Magnus P~lsson", tr, tg, tb);
    }

    if (graphics.onscreen(640 + game.creditposition))
    {
        graphics.Print(40, 640 + game.creditposition, "Rooms Named by", tr, tg, tb);
        graphics.bigprint(60, 650 + game.creditposition, "Bennett Foddy", tr, tg, tb);
    }

    if (graphics.onscreen(680 + game.creditposition))
    {
        graphics.Print(40, 680 + game.creditposition, "C++ Port by", tr, tg, tb);
        graphics.bigprint(60, 690 + game.creditposition, "Simon Roth", tr, tg, tb);
        graphics.bigprint(60, 710 + game.creditposition, "Ethan Lee", tr, tg, tb);
    }


    if (graphics.onscreen(740 + game.creditposition))
    {
        graphics.Print(40, 740 + game.creditposition, "Beta Testing by", tr, tg, tb);
        graphics.bigprint(60, 750 + game.creditposition, "Sam Kaplan", tr, tg, tb);
        graphics.bigprint(60, 770 + game.creditposition, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(800 + game.creditposition))
    {
        graphics.Print(40, 800 + game.creditposition, "Ending Picture by", tr, tg, tb);
        graphics.bigprint(60, 810 + game.creditposition, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(890 + game.creditposition)) graphics.bigprint( -1, 870 + game.creditposition, "Patrons", tr, tg, tb, true, 3);

    int creditOffset = 930;

    for (size_t i = 0; i < game.superpatrons.size(); i += 1)
    {
        if (graphics.onscreen(creditOffset + game.creditposition))
        {
            graphics.Print(-1, creditOffset + game.creditposition, game.superpatrons[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 10;
    if (graphics.onscreen(creditOffset + game.creditposition)) graphics.Print( -1, creditOffset + game.creditposition, "and", tr, tg, tb, true);
    creditOffset += 20;

    for (size_t i = 0; i < game.patrons.size(); i += 1)
    {
        if (graphics.onscreen(creditOffset + game.creditposition))
        {
            graphics.Print(-1, creditOffset + game.creditposition, game.patrons[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 20;
    if (graphics.onscreen(creditOffset + game.creditposition)) graphics.bigprint(40, creditOffset + game.creditposition, "GitHub Contributors", tr, tg, tb, true);
    creditOffset += 30;

    for (size_t i = 0; i < game.githubfriends.size(); i += 1)
    {
        if (graphics.onscreen(creditOffset + game.creditposition))
        {
            graphics.Print(-1, creditOffset + game.creditposition, game.githubfriends[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 140;
    if (graphics.onscreen(creditOffset + game.creditposition)) graphics.bigprint( -1, creditOffset + game.creditposition, "Thanks for playing!", tr, tg, tb, true, 2);

    graphics.drawfade();

    graphics.render();
}

void gamecompleterender2()
{
    FillRect(graphics.backBuffer, 0x000000);

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

    graphics.drawfade();

    graphics.render();
}

void gamerender()
{



    if(!game.blackout)
    {

        if (map.towermode)
        {
            if (!game.colourblindmode)
            {
                graphics.drawtowerbackground();
                graphics.drawtowermap();
            }
            else
            {
                FillRect(graphics.backBuffer,0x00000);
                graphics.drawtowermap_nobackground();
            }
        }
        else
        {
            if(!game.colourblindmode)
            {
                graphics.drawbackground(map.background);
            }
            else
            {
                FillRect(graphics.backBuffer,0x00000);
            }
            if (map.final_colormode)
            {
                graphics.drawfinalmap();
            }
            else
            {
                graphics.drawmap();
            }
        }


        if(!game.completestop)
        {
            for (size_t i = 0; i < obj.entities.size(); i++)
            {
                //Is this entity on the ground? (needed for jumping)
                if (obj.entitycollidefloor(i))
                {
                    obj.entities[i].onground = 2;
                }
                else
                {
                    obj.entities[i].onground--;
                }

                if (obj.entitycollideroof(i))
                {
                    obj.entities[i].onroof = 2;
                }
                else
                {
                    obj.entities[i].onroof--;
                }

                //Animate the entities
                obj.animateentities(i);
            }
        }

        graphics.drawentities();
        if (map.towermode)
        {
            graphics.drawtowerspikes();
        }

#if !defined(NO_CUSTOM_LEVELS)
        // Editor ghosts!
        if (game.ghostsenabled)
        {
            if (map.custommode && !map.custommodeforreal)
            {
                if (game.gametimer % 3 == 0)
                {
                    int i = obj.getplayer();
                    GhostInfo ghost;
                    ghost.rx = game.roomx-100;
                    ghost.ry = game.roomy-100;
                    if (i > -1)
                    {
                        ghost.x = obj.entities[i].xp;
                        ghost.y = obj.entities[i].yp;
                        ghost.col = obj.entities[i].colour;
                        ghost.frame = obj.entities[i].drawframe;
                    }
                    ed.ghosts.push_back(ghost);
                }
                if (ed.ghosts.size() > 100)
                {
                    ed.ghosts.erase(ed.ghosts.begin());
                }
            }
        }
#endif
    }

    if(map.extrarow==0 || (map.custommode && map.roomname!=""))
    {
        graphics.footerrect.y = 230;
        if (graphics.translucentroomname)
        {
            SDL_BlitSurface(graphics.footerbuffer, NULL, graphics.backBuffer, &graphics.footerrect);
        }
        else
        {
            FillRect(graphics.backBuffer, graphics.footerrect, 0);
        }

        if (map.finalmode)
        {
            map.glitchname = map.getglitchname(game.roomx, game.roomy);
            graphics.bprint(5, 231, map.glitchname, 196, 196, 255 - help.glow, true);
        }else{
            graphics.bprint(5, 231, map.roomname, 196, 196, 255 - help.glow, true);
        }
    }

    if (map.roomtexton)
    {
        //Draw room text!
        for (size_t i = 0; i < map.roomtext.size(); i++)
        {
            graphics.Print(map.roomtext[i].x*8, (map.roomtext[i].y*8), map.roomtext[i].text, 196, 196, 255 - help.glow);
        }
    }

#if !defined(NO_CUSTOM_LEVELS)
     if(map.custommode && !map.custommodeforreal && !game.advancetext){
        //Return to level editor
        graphics.bprintalpha(5, 5, loc::gettext("[Press ENTER to return to editor]"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), ed.returneditoralpha, false);
        if (ed.returneditoralpha > 0) {
            ed.returneditoralpha -= 15;
        }
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

    if (game.readytotele > 100 && !game.advancetext && game.hascontrol && !script.running && !game.intimetrial)
    {
        if(graphics.flipmode)
        {
            graphics.bprint(5, 20, loc::gettext("- Press ENTER to Teleport -"), game.readytotele - 20 - (help.glow / 2), game.readytotele - 20 - (help.glow / 2), game.readytotele, true);
        }
        else
        {
            graphics.bprint(5, 210, loc::gettext("- Press ENTER to Teleport -"), game.readytotele - 20 - (help.glow / 2), game.readytotele - 20 - (help.glow / 2), game.readytotele, true);
        }
    }

    if (game.swnmode)
    {
        if (game.swngame == 0)
        {
            std::string tempstring = help.timestring(game.swntimer);
            graphics.bigprint( -1, 20, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
        }
        else if (game.swngame == 1)
        {
            if (game.swnmessage == 0)
            {
                std::string tempstring = help.timestring(game.swntimer);
                graphics.Print( 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                #define BESTTIMELABEL loc::gettext("Best Time")
                graphics.Print( 320-graphics.len(BESTTIMELABEL)-8, 10, BESTTIMELABEL, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                #undef BESTTIMELABEL
                graphics.bigrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);

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
                graphics.Print( 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                if (int(game.deathseq / 5) % 2 == 1)
                {
                    #define BESTTIMELABEL loc::gettext("Best Time")
                    graphics.Print( 320-graphics.len(BESTTIMELABEL)-8, 10, BESTTIMELABEL, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                    #undef BESTTIMELABEL
                    graphics.bigrprint( 300, 24, tempstring, 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), false, 2);

                    graphics.bigprint( -1, 200, loc::gettext("New Record!"), 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), true, 2);
                }
            }
            else if (game.swnmessage >= 2)
            {
                game.swnmessage--;
                if (game.swnmessage == 2) game.swnmessage = 0;
                std::string tempstring = help.timestring(game.swntimer);
                graphics.Print( 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                #define BESTTIMELABEL loc::gettext("Best Time")
                graphics.Print( 320-graphics.len(BESTTIMELABEL)-8, 10, BESTTIMELABEL, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                #undef BESTTIMELABEL
                graphics.bigrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);

                if (int(game.swnmessage / 5) % 2 == 1)
                {
                    graphics.bigprint( -1, 200, loc::gettext("New Trophy!"), 220 - (help.glow), 128 - (help.glow), 128 - (help.glow / 2), true, 2);
                }
            }

            graphics.Print( 20, 228, loc::gettext("[Press ENTER to stop]"), 160 - (help.glow/2), 160 - (help.glow/2), 160 - (help.glow/2), true);
        }
        else if(game.swngame==2)
        {
            if (int(game.swndelay / 15) % 2 == 1 || game.swndelay >= 120)
            {
                if (graphics.flipmode)
                { // TODO LOC: wrapped bigprint. And how does flip mode even interact with the different print functions?
                    graphics.bigprint( -1, 30, "Survive for", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                    graphics.bigprint( -1, 10, "60 seconds!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                }
                else
                {
                    graphics.bigprint( -1, 10, "Survive for", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                    graphics.bigprint( -1, 30, "60 seconds!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                }
            }
        }
        else if(game.swngame==7)
        {
            if (game.swndelay >= 60)
            {
                graphics.bigprint( -1, 20, loc::gettext("SUPER GRAVITRON"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.Print( 240, 190, loc::gettext("Best Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                graphics.bigrprint( 300, 205, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
            }
            else	if (int(game.swndelay / 10) % 2 == 1)
            {
                graphics.bigprint( -1, 20, loc::gettext("SUPER GRAVITRON"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                graphics.bigprint( -1, 200, loc::gettext("GO!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 3);
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
                game.resetgameclock();
                if (int(game.timetrialcountdown / 4) % 2 == 0) graphics.bigprint( -1, 100, loc::gettext("Go!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 60)
            {
                graphics.bigprint( -1, 100, "1", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 90)
            {
                graphics.bigprint( -1, 100, "2", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 120)
            {
                graphics.bigprint( -1, 100, "3", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
        }
        else
        {
            //Draw OSD stuff
            std::string tempstring = loc::gettext("TIME:");
            int label_len = graphics.len(tempstring);
            graphics.bprint(6, 18, tempstring,  255,255,255);
            tempstring = loc::gettext("DEATH:");
            label_len = std::max(label_len, graphics.len(tempstring));
            graphics.bprint(6, 30, tempstring,  255, 255, 255);
            tempstring = loc::gettext("SHINY:");
            label_len = std::max(label_len, graphics.len(tempstring));
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
            char tempstring_c[40];
            sprintf(
                tempstring_c,
                loc::gettext("%d of %d").substr(0,39).c_str(),
                game.trinkets(), game.timetrialshinytarget
            );
            if(game.trinkets()<game.timetrialshinytarget)
            {
                graphics.bprint(8+label_len, 42, tempstring_c,  196, 80, 80);
            }
            else
            {
                graphics.bprint(8+label_len, 42, tempstring_c,  196, 196, 196);
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

    if (game.activeactivity > -1)
    {
        game.activity_lastprompt = obj.blocks[game.activeactivity].prompt;
        game.activity_r = obj.blocks[game.activeactivity].r;
        game.activity_g = obj.blocks[game.activeactivity].g;
        game.activity_b = obj.blocks[game.activeactivity].b;
        if(game.act_fade<5) game.act_fade=5;
        if(game.act_fade<10)
        {
            game.act_fade++;
        }
        graphics.drawtextbox(16, 4, 36, 3, game.activity_r*(game.act_fade/10.0f), game.activity_g*(game.act_fade/10.0f), game.activity_b*(game.act_fade/10.0f));
        graphics.Print(5, 12, game.activity_lastprompt, game.activity_r*(game.act_fade/10.0f), game.activity_g*(game.act_fade/10.0f), game.activity_b*(game.act_fade/10.0f), true);
    }
    else
    {
        if(game.act_fade>5)
        {
            graphics.drawtextbox(16, 4, 36, 3, game.activity_r*(game.act_fade/10.0f), game.activity_g*(game.act_fade/10.0f), game.activity_b*(game.act_fade/10.0f));
            graphics.Print(5, 12, game.activity_lastprompt, game.activity_r*(game.act_fade/10.0f), game.activity_g*(game.act_fade/10.0f), game.activity_b*(game.act_fade/10.0f), true);
            game.act_fade--;
        }
    }

    if (obj.trophytext > 0)
    {
        graphics.drawtrophytext();
        obj.trophytext--;
    }


    graphics.renderwithscreeneffects();
}

void maprender()
{
    //draw screen alliteration
    //Roomname:
    int temp = map.area(game.roomx, game.roomy);
    if (temp < 2 && !map.custommode && graphics.fademode==0)
    { // TODO LOC
        if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111)
        {
            graphics.Print(5, 2, "The Ship", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(5, 2, "Dimension VVVVVV", 196, 196, 255 - help.glow, true);
        }
    }
    else
    {
      if (map.finalmode){
        map.glitchname = map.getglitchname(game.roomx, game.roomy);
        graphics.Print(5, 2, map.glitchname, 196, 196, 255 - help.glow, true);
      }else{
        graphics.Print(5, 2, map.roomname, 196, 196, 255 - help.glow, true);
      }
    }

    //Background color
    FillRect(graphics.backBuffer,0, 12, 320, 240, 10, 24, 26 );

    graphics.crewframedelay--;
    if (graphics.crewframedelay <= 0)
    {
        graphics.crewframedelay = 8;
        graphics.crewframe = (graphics.crewframe + 1) % 2;
    }



    //Menubar:
    graphics.drawtextbox( -10, 212, 42, 3, 65, 185, 207);
    switch(game.menupage)
    {
    case 0:
        graphics.map_tab(0, loc::gettext("MAP"), true);
        if (game.insecretlab)
        {
            graphics.map_tab(1, loc::gettext("GRAV"));
        }
        else if (obj.flags[67] && !map.custommode)
        {
            graphics.map_tab(1, loc::gettext("SHIP"));
        }
        else
        {
            graphics.map_tab(1, loc::gettext("CREW"));
        }
        graphics.map_tab(2, loc::gettext("STATS"));
        graphics.map_tab(3, loc::gettext("SAVE"));

        if (map.finalmode || (map.custommode&&!map.customshowmm))
        {
            //draw the map image
            graphics.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
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
        else if(map.custommode)
        {
          //draw the map image
          graphics.drawcustompixeltextbox(35+map.custommmxoff, 16+map.custommmyoff, map.custommmxsize+10, map.custommmysize+10, (map.custommmxsize+10)/8, (map.custommmysize+10)/8, 65, 185, 207,4,0);
          graphics.drawpartimage(12, 40+map.custommmxoff, 21+map.custommmyoff, map.custommmxsize,map.custommmysize);

          //Black out here
          if(map.customzoom==4){
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(map.explored[i+(j*20)]==0){
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
                if(map.explored[i+(j*20)]==0){
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
                if(map.explored[i+(j*20)]==0){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 12), map.custommmyoff+21 + (j * 9), false);
                }
              }
            }
          }

          if (map.cursorstate == 0){
            map.cursordelay++;
            if (map.cursordelay > 10){
              map.cursorstate = 1;
              map.cursordelay = 0;
            }
          }else if (map.cursorstate == 1){
            map.cursordelay++;
            if (map.cursordelay > 30) map.cursorstate = 2;
          }else if (map.cursorstate == 2){
            map.cursordelay++;
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
        }
        else
        {
            //draw the map image
            graphics.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
            graphics.drawimage(1, 40, 21, false);

            //black out areas we can't see yet
            for (int j = 0; j < 20; j++)
            {
                for (int i = 0; i < 20; i++)
                {
                    if(map.explored[i+(j*20)]==0)
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
                if (map.cursorstate == 0)
                {
                    map.cursordelay++;
                    if (map.cursordelay > 10)
                    {
                        map.cursorstate = 1;
                        map.cursordelay = 0;
                    }
                }
                else if (map.cursorstate == 1)
                {
                    map.cursordelay++;
                    if (int(map.cursordelay / 4) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) , 21 , 12, 180, 255,255,255);
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4, 255,255,255);
                    }
                    if (map.cursordelay > 30) map.cursorstate = 2;
                }
                else if (map.cursorstate == 2)
                {
                    map.cursordelay++;
                    if (int(map.cursordelay / 15) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4,16, 245 - (help.glow), 245 - (help.glow));
                    }
                }
            }
            else
            {
                if (map.cursorstate == 0)
                {
                    map.cursordelay++;
                    if (map.cursordelay > 10)
                    {
                        map.cursorstate = 1;
                        map.cursordelay = 0;
                    }
                }
                else if (map.cursorstate == 1)
                {
                    map.cursordelay++;
                    if (int(map.cursordelay / 4) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) , 21 + ((game.roomy - 100) * 9) , 12 , 9 , 255,255,255);
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 255,255,255);
                    }
                    if (map.cursordelay > 30) map.cursorstate = 2;
                }
                else if (map.cursorstate == 2)
                {
                    map.cursordelay++;
                    if (int(map.cursordelay / 15) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));
                    }
                }
            }

            //draw legend details
            for (size_t i = 0; i < map.teleporters.size(); i++)
            {
                if (map.showteleporters && map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)] > 0)
                {
                    int temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
                    if (graphics.flipmode) temp += 3;
                    graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
                }
                else if(map.showtargets && map.explored[map.teleporters[i].x+(20*map.teleporters[i].y)]==0)
                {
                    int temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
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
                        graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),	temp);
                    }
                }
            }
        }
        break;
    case 1:
        if (game.insecretlab)
        {
            graphics.map_tab(0, loc::gettext("MAP"));
            graphics.map_tab(1, loc::gettext("GRAV"), true);
            graphics.map_tab(2, loc::gettext("STATS"));
            graphics.map_tab(3, loc::gettext("SAVE"));

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
            graphics.map_tab(0, loc::gettext("MAP"));
            graphics.map_tab(1, loc::gettext("SHIP"), true);
            graphics.map_tab(2, loc::gettext("STATS"));
            graphics.map_tab(3, loc::gettext("SAVE"));

            graphics.PrintWrap(0, 105, loc::gettext("Press ACTION to warp to the ship."), 196, 196, 255 - help.glow, true);
        }
#if !defined(NO_CUSTOM_LEVELS)
        else if(map.custommode){
            graphics.map_tab(0, loc::gettext("MAP"));
            graphics.map_tab(1, loc::gettext("CREW"), true);
            graphics.map_tab(2, loc::gettext("STATS"));
            graphics.map_tab(3, loc::gettext("SAVE"));

            if (graphics.flipmode)
            {
                graphics.bigprint( -1, 220-45, ed.ListOfMetaData[game.playcustomlevel].title, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 220-70, "by " + ed.ListOfMetaData[game.playcustomlevel].creator, 196, 196, 255 - help.glow, true); // TODO LOC repeat that thing from above
                graphics.Print( -1, 220-80, ed.ListOfMetaData[game.playcustomlevel].website, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 220-100, ed.ListOfMetaData[game.playcustomlevel].Desc1, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 220-110, ed.ListOfMetaData[game.playcustomlevel].Desc2, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 220-120, ed.ListOfMetaData[game.playcustomlevel].Desc3, 196, 196, 255 - help.glow, true);

                if(ed.numcrewmates()-game.crewmates()==1){ // TODO LOC
                    graphics.Print(1,220-165, help.number(ed.numcrewmates()-game.crewmates())+ " crewmate remains", 196, 196, 255 - help.glow, true);
                }else if(ed.numcrewmates()-game.crewmates()>0){
                    graphics.Print(1,220-165, help.number(ed.numcrewmates()-game.crewmates())+ " crewmates remain", 196, 196, 255 - help.glow, true);
                }
            }
            else
            {
                graphics.bigprint( -1, 45, ed.ListOfMetaData[game.playcustomlevel].title, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 70, "by " + ed.ListOfMetaData[game.playcustomlevel].creator, 196, 196, 255 - help.glow, true); // TODO LOC :D
                graphics.Print( -1, 80, ed.ListOfMetaData[game.playcustomlevel].website, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 100, ed.ListOfMetaData[game.playcustomlevel].Desc1, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 110, ed.ListOfMetaData[game.playcustomlevel].Desc2, 196, 196, 255 - help.glow, true);
                graphics.Print( -1, 120, ed.ListOfMetaData[game.playcustomlevel].Desc3, 196, 196, 255 - help.glow, true);

                if(ed.numcrewmates()-game.crewmates()==1){ // TODO LOC :D
                    graphics.Print(1,165, help.number(ed.numcrewmates()-game.crewmates())+ " crewmate remains", 196, 196, 255 - help.glow, true);
                }else if(ed.numcrewmates()-game.crewmates()>0){
                    graphics.Print(1,165, help.number(ed.numcrewmates()-game.crewmates())+ " crewmates remain", 196, 196, 255 - help.glow, true);
                }
            }
        }
#endif
        else
        {
            graphics.map_tab(0, loc::gettext("MAP"));
            graphics.map_tab(1, loc::gettext("CREW"), true);
            graphics.map_tab(2, loc::gettext("STATS"));
            graphics.map_tab(3, loc::gettext("SAVE"));

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
        graphics.map_tab(0, loc::gettext("MAP"));
        if (game.insecretlab)
        {
            graphics.map_tab(1, loc::gettext("GRAV"));
        }
        else if (obj.flags[67] && !map.custommode)
        {
            graphics.map_tab(1, loc::gettext("SHIP"));
        }
        else
        {
            graphics.map_tab(1, loc::gettext("CREW"));
        }
        graphics.map_tab(2, loc::gettext("STATS"), true);
        graphics.map_tab(3, loc::gettext("SAVE"));

#if !defined(NO_CUSTOM_LEVELS)
        if(map.custommode)
        {
          if (graphics.flipmode)
          {
              graphics.Print(0, 164, loc::gettext("[Trinkets found]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 152, help.number(game.trinkets()) + " out of " + help.number(ed.numtrinkets()), 96,96,96, true);

              graphics.Print(0, 114, loc::gettext("[Number of Deaths]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 102,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 64, loc::gettext("[Time Taken]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 52, game.timestring(),  96, 96, 96, true);
          }
          else
          {
              graphics.Print(0, 52, loc::gettext("[Trinkets found]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 64, help.number(game.trinkets()) + " out of "+help.number(ed.numtrinkets()), 96,96,96, true);

              graphics.Print(0, 102, loc::gettext("[Number of Deaths]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 114,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 152, loc::gettext("[Time Taken]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 164, game.timestring(),  96, 96, 96, true);
          }
        }
        else
#endif
        {
          if (graphics.flipmode)
          {
              graphics.Print(0, 164, loc::gettext("[Trinkets found]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 152, help.number(game.trinkets()) + " out of Twenty", 96,96,96, true);

              graphics.Print(0, 114, loc::gettext("[Number of Deaths]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 102,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 64, loc::gettext("[Time Taken]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 52, game.timestring(),  96, 96, 96, true);
          }
          else
          {
              graphics.Print(0, 52, loc::gettext("[Trinkets found]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 64, help.number(game.trinkets()) + " out of Twenty", 96,96,96, true);

              graphics.Print(0, 102, loc::gettext("[Number of Deaths]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 114,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 152, loc::gettext("[Time Taken]"), 196, 196, 255 - help.glow, true);
              graphics.Print(0, 164, game.timestring(),  96, 96, 96, true);
          }
        }
        break;
    case 3:
        graphics.map_tab(0, loc::gettext("MAP"));
        if (game.insecretlab)
        {
            graphics.map_tab(1, loc::gettext("GRAV"));
        }
        else if (obj.flags[67] && !map.custommode)
        {
            graphics.map_tab(1, loc::gettext("SHIP"));
        }
        else
        {
            graphics.map_tab(1, loc::gettext("CREW"));
        }
        graphics.map_tab(2, loc::gettext("STATS"));
        graphics.map_tab(3, loc::gettext("SAVE"), true);

        if (game.inintermission)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Level Replay"), 146, 146, 180, true);
        }
        else if (game.nodeathmode)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in No Death Mode"), 146, 146, 180, true);
        }
        else if (game.intimetrial)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Time Trial"), 146, 146, 180, true);
        }
        else if (game.insecretlab)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Secret Lab"), 146, 146, 180, true);
        }
        else if (map.custommode)
        {
            if (game.gamesaved)
            {
                graphics.PrintWrap(0, 36, loc::gettext("Game saved ok!"), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2), true);

                graphics.drawpixeltextbox(25, 65, 270, 90, 34,12, 65, 185, 207,0,4);

                if (graphics.flipmode)
                {
                    graphics.Print(0, 122, game.customleveltitle, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    graphics.Print(160 - 84, 78, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    graphics.Print(160 + 40, 78, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawspritesetcol(50, 74, 50, 18);
                    graphics.drawspritesetcol(175, 74, 22, 18);
                }
                else
                {
                    graphics.Print(0, 90, game.customleveltitle, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    graphics.Print(160 - 84, 132, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    graphics.Print(160 + 40, 132, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawspritesetcol(50, 126, 50, 18);
                    graphics.drawspritesetcol(175, 126, 22, 18);
                }
            }
            else
            {
                graphics.PrintWrap(0, 80, loc::gettext("[Press ACTION to save your game]"), 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);
            }
        }
        else
        {
            if (graphics.flipmode) // TODO LOC: flipmode
            {
                graphics.Print(0, 186, "(Note: The game is autosaved", 146, 146, 180, true);
                graphics.Print(0, 174, "at every teleporter.)", 146, 146, 180, true);
            }
            else
            {
                graphics.Print(0, 174, "(Note: The game is autosaved", 146, 146, 180, true);
                graphics.Print(0, 186, "at every teleporter.)", 146, 146, 180, true);
            }

            if (game.gamesaved)
            {
                graphics.PrintWrap(0, 36, loc::gettext("Game saved ok!"), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2), true);

                graphics.drawpixeltextbox(25, 65, 270, 90, 34,12, 65, 185, 207,0,4);

                if (graphics.flipmode)
                {
                    graphics.Print(0, 132, game.savearea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    for (int i = 0; i < 6; i++)
                    {
                        graphics.drawcrewman(169-(3*42)+(i*42), 98, i, game.crewstats[i], true);
                    }
                    graphics.Print(160 - 84, 78, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    graphics.Print(160 + 40, 78, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawspritesetcol(50, 74, 50, 18);
                    graphics.drawspritesetcol(175, 74, 22, 18);
                }
                else
                {
                    graphics.Print(0, 80, game.savearea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    for (int i = 0; i < 6; i++)
                    {
                        graphics.drawcrewman(169-(3*42)+(i*42), 95, i, game.crewstats[i], true);
                    }
                    graphics.Print(160 - 84, 132, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    graphics.Print(160 + 40, 132, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawspritesetcol(50, 126, 50, 18);
                    graphics.drawspritesetcol(175, 126, 22, 18);
                }
            }
            else
            {
                graphics.PrintWrap(0, 80, loc::gettext("[Press ACTION to save your game]"), 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);

                if (game.quicksummary != "")
                {
                    if (graphics.flipmode)
                    {
                        graphics.Print(0, 110, loc::gettext("Last Save:"), 164 - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                        graphics.Print(0, 100, game.quicksummary, 164  - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                    }
                    else
                    {
                        graphics.Print(0, 100, loc::gettext("Last Save:"), 164 - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                        graphics.Print(0, 110, game.quicksummary, 164  - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                    }
                }
            }
        }
        break;
    case 10:
        graphics.Print(128, 220, "[ QUIT ]", 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            if (game.intimetrial || game.insecretlab || game.nodeathmode)
            {
                graphics.Print(0, 135, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 142, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 130, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80-16, 88, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 76, "yes, quit to menu",  96, 96, 96);
        }
        else
        {

            if (game.intimetrial || game.insecretlab || game.nodeathmode)
            {
                graphics.Print(0, 80, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 76, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 88, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80-16, 130, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 142, "yes, quit to menu",  96, 96, 96);

        }
        break;
    case 11:
        graphics.Print(128, 220, "[ QUIT ]", 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            if (game.intimetrial || game.insecretlab || game.nodeathmode)
            {
                graphics.Print(0, 135, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 142, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 130, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80, 88, "no, keep playing", 96,96,96);
            graphics.Print(80+32-16, 76, "[ YES, QUIT TO MENU ]",  196, 196, 255 - help.glow);
        }
        else
        {
            if (game.intimetrial || game.insecretlab || game.nodeathmode)
            {
                graphics.Print(0, 80, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 76, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 88, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80, 130, "no, keep playing", 96,96,96);
            graphics.Print(80+32-16, 142, "[ YES, QUIT TO MENU ]", 196, 196, 255 - help.glow);
        }
        break;
    case 20:
        graphics.Print(128, 220, "[ GRAVITRON ]", 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            graphics.Print(0, 76, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(80-16, 142, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 130, "yes, return",  96, 96, 96);
        }
        else
        {
            graphics.Print(0, 76, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(80-16, 130, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 142, "yes, return",  96, 96, 96);
        }

        break;
    case 21:
        graphics.Print(128, 220, "[ GRAVITRON ]", 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            graphics.Print(0, 76, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(80, 142, "no, keep playing", 96, 96, 96);
            graphics.Print(80 + 32-16, 130, "[ YES, RETURN ]",  196, 196, 255 - help.glow);
        }
        else
        {
            graphics.Print(0, 76, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(80, 130, "no, keep playing", 96, 96, 96);
            graphics.Print(80 + 32-16, 142, "[ YES, RETURN ]",  196, 196, 255 - help.glow);
        }

    }




    if (graphics.fademode == 3 || graphics.fademode == 5)
    {
        graphics.drawfade();
    }

    if (graphics.resumegamemode)
    {
        graphics.menuoffset += 25;
        if (map.extrarow)
        {
            if (graphics.menuoffset >= 230)
            {
                graphics.menuoffset = 230;
                //go back to gamemode!
                game.mapheld = true;
                game.gamestate = GAMEMODE;
            }
        }
        else
        {
            if (graphics.menuoffset >= 240)
            {
                graphics.menuoffset = 240;
                //go back to gamemode!
                game.mapheld = true;
                game.gamestate = GAMEMODE;
            }
        }
        graphics.menuoffrender();
    }
    else if (graphics.menuoffset > 0)
    {
        graphics.menuoffset -= 25;
        if (graphics.menuoffset < 0) graphics.menuoffset = 0;
        graphics.menuoffrender();
    }
    else
    {
        graphics.renderwithscreeneffects();
    }
}

void teleporterrender()
{
    int tempx;
    int tempy;
    //draw screen alliteration
    //Roomname:
    int temp = map.area(game.roomx, game.roomy);
    if (temp < 2 && !map.custommode && graphics.fademode==0)
    {
        if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111)
        {
            graphics.Print(5, 2, "The Ship", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(5, 2, "Dimension VVVVVV", 196, 196, 255 - help.glow, true);
        }
    }
    else
    {
        graphics.Print(5, 2, map.roomname, 196, 196, 255 - help.glow, true);
    }

    //Background color
    FillRect(graphics.backBuffer, 0, 12, 320, 240, 10, 24, 26);

    //draw the map image
    graphics.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
    graphics.drawimage(1, 40, 21, false);
    //black out areas we can't see yet
    for (int j = 0; j < 20; j++)
    {
        for (int i = 0; i < 20; i++)
        {
            if(map.explored[i+(j*20)]==0)
            {
                //graphics.drawfillrect(10 + (i * 12), 21 + (j * 9), 12, 9, 16, 16, 16);
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
        int tempx = map.teleporters[game.teleport_to_teleporter].x;
        int tempy = map.teleporters[game.teleport_to_teleporter].y;
        graphics.drawrect(40 + (tempx * 12) + 1, 21 + (tempy * 9) + 1, 12 - 2, 9 - 2, 245 - (help.glow * 2), 16, 16);
        graphics.drawrect(40 + (tempx * 12) + 3, 21 + (tempy * 9) + 3, 12 - 6, 9 - 6, 245 - (help.glow * 2), 16, 16);
    }

    //draw legend details
    for (size_t i = 0; i < map.teleporters.size(); i++)
    {
        if (map.showteleporters && map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)] > 0)
        {
            temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
            if (graphics.flipmode) temp += 3;
            graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
        }
        else if(map.showtargets && map.explored[map.teleporters[i].x+(20*map.teleporters[i].y)]==0)
        {
            temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
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
                graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),	temp);
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
        //Instructions!
        graphics.Print(5, 210, "Press Left/Right to choose a Teleporter", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
        graphics.Print(5, 225, "Press ENTER to Teleport", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }

    graphics.drawgui();

    if (graphics.flipmode)
    {
        if (game.advancetext) graphics.bprint(5, 228, "- Press ACTION to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }
    else
    {
        if (game.advancetext) graphics.bprint(5, 5, "- Press ACTION to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }


    if (graphics.resumegamemode)
    {
        graphics.menuoffset += 25;
        if (map.extrarow)
        {
            if (graphics.menuoffset >= 230)
            {
                graphics.menuoffset = 230;
                //go back to gamemode!
                game.mapheld = true;
                game.gamestate = GAMEMODE;
            }
        }
        else
        {
            if (graphics.menuoffset >= 240)
            {
                graphics.menuoffset = 240;
                //go back to gamemode!
                game.mapheld = true;
                game.gamestate = GAMEMODE;
            }
        }
        graphics.menuoffrender();
    }
    else if (graphics.menuoffset > 0)
    {
        graphics.menuoffset -= 25;
        if (graphics.menuoffset < 0) graphics.menuoffset = 0;
        graphics.menuoffrender();
    }
    else
    {
        graphics.render();
    }
}
