#include "Localization.h"

#include <stdio.h>
#include <tinyxml2.h>
#include <utf8/unchecked.h>

#include "Constants.h"
#include "FileSystemUtils.h"
//#include "Graphics.h"
#include "UtilityClass.h"
#include "Vlogging.h"

namespace loc
{
    std::string lang = "en";
    LangMeta langmeta;
    bool test_mode = false;

    // language screen list
    std::vector<LangMeta> languagelist;
    int languagelist_curlang;
    bool show_translator_menu;

    bool inited = false;

    Textbook textbook_main;

    hashmap* map_translation;
    hashmap* map_translation_cutscene;
    std::string number[102];

#define MAP_MAX_X 54
#define MAP_MAX_Y 56
    char* translation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
    char* explanation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
    hashmap* map_translation_roomnames_special;

    bool load_doc(const std::string& cat, tinyxml2::XMLDocument& doc, const std::string& langcode = lang)
    {
        if (!FILESYSTEM_loadTiXml2Document(("lang/" + langcode + "/" + cat + ".xml").c_str(), doc))
        {
            vlog_info("Could not load language file %s/%s.", langcode.c_str(), cat.c_str());
            return false;
        }
        return true;
    }

    void loadmeta(LangMeta& meta, const std::string& langcode = lang)
    {
        meta.active = true;
        meta.code = langcode;
        meta.autowordwrap = true;
        meta.toupper = true;
        meta.toupper_i_dot = false;
        meta.toupper_lower_escape_char = false;

        tinyxml2::XMLDocument doc;
        if (!load_doc("meta", doc, langcode))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText();
            if (pText == NULL)
            {
                pText = "";
            }

            if (SDL_strcmp(pKey, "active") == 0)
                meta.active = atoi(pText);
            else if (SDL_strcmp(pKey, "nativename") == 0)
                meta.nativename = std::string(pText);
            else if (SDL_strcmp(pKey, "credit") == 0)
                meta.credit = std::string(pText);
            else if (SDL_strcmp(pKey, "autowordwrap") == 0)
                meta.autowordwrap = atoi(pText);
            else if (SDL_strcmp(pKey, "toupper") == 0)
                meta.toupper = atoi(pText);
            else if (SDL_strcmp(pKey, "toupper_i_dot") == 0)
                meta.toupper_i_dot = atoi(pText);
            else if (SDL_strcmp(pKey, "toupper_lower_escape_char") == 0)
                meta.toupper_lower_escape_char = atoi(pText);
        }
    }

    void textbook_init(Textbook& textbook)
    {
        textbook.pages_used = 0;
    }

    void textbook_clear(Textbook& textbook)
    {
        for (short p = 0; p < textbook.pages_used; p++)
        {
            SDL_free(textbook.page[p]);
        }
        textbook.pages_used = 0;
    }

    char* textbook_store(Textbook& textbook, const char* text)
    {
        if (text == NULL)
        {
            return NULL;
        }

        size_t text_len = SDL_strlen(text)+1;

        if (text_len > TEXTBOOK_PAGE_SIZE)
        {
            vlog_warn(
                "Cannot store string of %ld bytes in Textbook, max page size is %d",
                text_len,
                TEXTBOOK_PAGE_SIZE
            );
            return NULL;
        }

        /* Find a suitable page to place our text on */
        short found_page = -1;
        for (short p = 0; p < textbook.pages_used; p++)
        {
            size_t free = TEXTBOOK_PAGE_SIZE - textbook.page_len[p];

            if (text_len <= free)
            {
                found_page = p;
                break;
            }
        }

        if (found_page == -1)
        {
            /* Create a new page then */
            found_page = textbook.pages_used;

            if (found_page >= TEXTBOOK_MAX_PAGES)
            {
                vlog_warn(
                    "Textbook is full! %hd pages used (%d chars per page)",
                    textbook.pages_used,
                    TEXTBOOK_PAGE_SIZE
                );
                return NULL;
            }

            textbook.page[found_page] = (char*) SDL_malloc(TEXTBOOK_PAGE_SIZE);
            if (textbook.page[found_page] == NULL)
            {
                return NULL;
            }

            textbook.page_len[found_page] = 0;
            textbook.pages_used++;
        }

        size_t cursor = textbook.page_len[found_page];
        char* added_text = &textbook.page[found_page][cursor];
        SDL_memcpy(added_text, text, text_len);
        textbook.page_len[found_page] += text_len;

        return added_text;
    }

    void map_store_text(Textbook& textbook, hashmap* map, const char* eng, const char* tra)
    {
        if (eng == NULL || tra == NULL)
        {
            return;
        }
        const char* tb_eng = textbook_store(textbook, eng);
        const char* tb_tra;
        if (test_mode)
        {
            tb_tra = textbook_store(textbook, ("✓" + std::string(tra[0] == '\0' ? eng : tra)).c_str());
        }
        else
        {
            tb_tra = textbook_store(textbook, tra);
        }

        if (tb_eng == NULL || tb_tra == NULL)
        {
            return;
        }

        hashmap_set(map, (void*) tb_eng, SDL_strlen(tb_eng), (uintptr_t) tb_tra);
    }

    const char* map_store_404(hashmap* map, const char* eng)
    {
        /* Store a "string not found" translation, only called in test mode */
        const char* tb_eng = textbook_store(textbook_main, eng);
        const char* tb_tra = textbook_store(textbook_main, ("❌" + std::string(eng)).c_str());

        if (tb_eng == NULL || tb_tra == NULL)
        {
            return eng;
        }

        hashmap_set(map, (void*) tb_eng, SDL_strlen(tb_eng), (uintptr_t) tb_tra);

        return tb_tra;
    }

    void callback_free_map_value(void* key, size_t ksize, uintptr_t value, void* usr)
    {
        hashmap_free((hashmap*) value);
    }

    void resettext(void)
    {
        // Reset/Initialize strings
        if (inited)
        {
            hashmap_free(map_translation);
            hashmap_iterate(map_translation_cutscene, callback_free_map_value, NULL);
            hashmap_free(map_translation_cutscene);
            hashmap_free(map_translation_roomnames_special);

            textbook_clear(textbook_main);
        }
        inited = true;

        textbook_init(textbook_main);

        map_translation = hashmap_create();
        map_translation_cutscene = hashmap_create();

        for (size_t i = 0; i <= 101; i++)
        {
            number[i] = "";
        }

        for (size_t y = 0; y <= MAP_MAX_Y; y++)
        {
            for (size_t x = 0; x < MAP_MAX_X; x++)
            {
                translation_roomnames[y][x] = NULL;
                explanation_roomnames[y][x] = NULL;
            }
        }

        map_translation_roomnames_special = hashmap_create();
    }

    void loadtext_strings(void)
    {
        tinyxml2::XMLDocument doc;
        if (!load_doc("strings", doc))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText();
            if (pText == NULL)
            {
                pText = "";
            }

            if (SDL_strcmp(pKey, "string") == 0)
            {
                map_store_text(textbook_main, map_translation, pElem->Attribute("english"), pText);
            }
        }
    }

    void loadtext_cutscenes(void)
    {
        tinyxml2::XMLDocument doc;
        if (!load_doc("cutscenes", doc))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "cutscene") == 0)
            {
                const char* script_id = textbook_store(textbook_main, pElem->Attribute("id"));

                hashmap* cutscene_map = hashmap_create();
                hashmap_set_free(
                    map_translation_cutscene,
                    (void*) script_id,
                    SDL_strlen(script_id),
                    (uintptr_t) cutscene_map,
                    callback_free_map_value,
                    NULL
                );

                for (tinyxml2::XMLElement* subElem = pElem->FirstChildElement(); subElem; subElem=subElem->NextSiblingElement())
                {
                    const char* pSubKey = subElem->Value();
                    const char* pSubText = subElem->GetText();
                    if (pSubText == NULL)
                    {
                        pSubText = "";
                    }

                    if (SDL_strcmp(pSubKey, "dialogue") == 0)
                    {
                        map_store_text(textbook_main, cutscene_map, subElem->Attribute("english"), pSubText);
                    }
                }
            }
        }
    }

    void loadtext_numbers(void)
    {
        tinyxml2::XMLDocument doc;
        if (!load_doc("numbers", doc))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText();
            if (pText == NULL)
            {
                pText = "";
            }

            if (SDL_strcmp(pKey, "number") == 0)
            {
                int value = atoi(pElem->Attribute("value"));
                if (value >= 0 && value <= 101)
                {
                    number[value] = std::string(pText);
                }
            }
        }
    }

    bool fix_room_coords(int* roomx, int* roomy)
    {
        *roomx %= 100;
        *roomy %= 100;

        return !(*roomx < 0 || *roomy < 0 || *roomx > MAP_MAX_X || *roomy > MAP_MAX_Y);
    }

    bool store_roomname_translation(bool custom_level, int roomx, int roomy, const char* tra, const char* explanation)
    {
        if (custom_level)
        {
            vlog_error("Custom level room names NYI");
            return false;
        }

        if (!fix_room_coords(&roomx, &roomy))
        {
            return false;
        }

        if (tra != NULL)
        {
            translation_roomnames[roomy][roomx] = textbook_store(textbook_main, tra);
        }
        if (explanation != NULL)
        {
            explanation_roomnames[roomy][roomx] = textbook_store(textbook_main, explanation);
        }

        return true;
    }

    void loadtext_roomnames(void)
    {
        tinyxml2::XMLDocument doc;
        if (!load_doc("roomnames", doc))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText();
            if (pText == NULL)
            {
                pText = "";
            }

            if (SDL_strcmp(pKey, "roomname") == 0)
            {
                int x = pElem->IntAttribute("x", -1);
                int y = pElem->IntAttribute("y", -1);

                store_roomname_translation(
                    false,
                    x,
                    y,
                    pText,
                    show_translator_menu ? pElem->Attribute("explanation") : NULL
                );
            }
        }
    }

    void loadtext_roomnames_special(void)
    {
        tinyxml2::XMLDocument doc;
        if (!load_doc("roomnames_special", doc))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText();
            if (pText == NULL)
            {
                pText = "";
            }

            if (SDL_strcmp(pKey, "roomname") == 0)
            {
                map_store_text(textbook_main, map_translation_roomnames_special, pElem->Attribute("english"), pText);
            }
        }
    }

    void loadtext(void)
    {
        resettext();
        loadmeta(langmeta);

        if (lang == "en" && !test_mode)
        {
            if (show_translator_menu)
            {
                // We may still need the room name explanations
                loadtext_roomnames();
            }

            return;
        }

        loadtext_strings();
        loadtext_cutscenes();
        loadtext_numbers();
        loadtext_roomnames();
        loadtext_roomnames_special();
    }

    void loadlanguagelist(void)
    {
        // Load the list of languages for the language screen
        languagelist.clear();

        std::vector<std::string> codes = FILESYSTEM_getLanguageCodes();
        size_t opt = 0;
        for (size_t i = 0; i < codes.size(); i++)
        {
            LangMeta meta;
            loadmeta(meta, codes[i]);
            if (meta.active)
            {
                languagelist.push_back(meta);

                if (lang == codes[i])
                    languagelist_curlang = opt;
                opt++;
            }
        }
    }

    void sync_lang_file(const std::string& langcode)
    {
        // Update translation files for the given language with new strings from template.
        // This basically takes the template, fills in existing translations, and saves.
        vlog_info("Syncing %s with templates...", langcode.c_str());

        lang = langcode;
        loadtext();

        tinyxml2::XMLDocument doc;
        if (!load_doc("strings", doc, "en"))
        {
            return;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "string") == 0)
            {
                const char* eng = pElem->Attribute("english");
                uintptr_t ptr_tra;
                bool found = hashmap_get(map_translation, (void*) eng, SDL_strlen(eng), &ptr_tra);
                const char* tra = (const char*) ptr_tra;
                if (!found)
                {
                    tra = "";
                }
                pElem->SetText(tra);
            }
        }

        // Writing to main lang dir
        FILESYSTEM_saveTiXml2Document((langcode + "/strings.xml").c_str(), doc);
    }

    void sync_lang_files(void)
    {
        std::string oldlang = lang;
        if (!FILESYSTEM_setLangWriteDir())
        {
            vlog_error("Cannot set write dir to lang dir, not syncing language files");
            return;
        }

        for (size_t i = 0; i < languagelist.size(); i++)
        {
            if (languagelist[i].code != "en")
                sync_lang_file(languagelist[i].code);
        }

        FILESYSTEM_restoreWriteDir();
        lang = oldlang;
        loadtext();
    }

    bool save_roomname_to_file(const std::string& langcode, bool custom_level, int roomx, int roomy, const char* tra, const char* explanation)
    {
        if (custom_level)
        {
            vlog_error("Custom level room names NYI");
            return false;
        }

        if (!fix_room_coords(&roomx, &roomy))
        {
            return false;
        }

        tinyxml2::XMLDocument doc;
        if (!load_doc("roomnames", doc, langcode))
        {
            return false;
        }

        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        {
            pElem=hDoc.FirstChildElement().ToElement();
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        bool found = false;
        for (pElem = hRoot.FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "roomname") == 0)
            {
                int x = pElem->IntAttribute("x", -1);
                int y = pElem->IntAttribute("y", -1);

                if (x == roomx && y == roomy)
                {
                    if (explanation != NULL)
                    {
                        pElem->SetAttribute("explanation", explanation);
                    }
                    if (tra != NULL)
                    {
                        pElem->SetText(tra);
                    }
                    found = true;
                }
            }
        }

        if (!found)
        {
            vlog_error("Could not find room %d,%d in language file to replace!", roomx, roomy);
            return false;
        }

        if (!FILESYSTEM_setLangWriteDir())
        {
            vlog_error("Cannot set write dir to lang dir, so room name can't be saved");
            return false;
        }
        FILESYSTEM_saveTiXml2Document((langcode + "/roomnames.xml").c_str(), doc);
        FILESYSTEM_restoreWriteDir();
        return store_roomname_translation(custom_level, roomx, roomy, tra, explanation);
    }

    bool save_roomname_explanation_to_files(bool custom_level, int roomx, int roomy, const char* explanation)
    {
        bool any = false;
        bool success = true;
        for (size_t i = 0; i < languagelist.size(); i++)
        {
            any = true;
            if (!save_roomname_to_file(languagelist[i].code, custom_level, roomx, roomy, NULL, explanation))
            {
                success = false;
                vlog_warn("Could not save room name explanation to language %s", languagelist[i].code.c_str());
            }
        }

        return any && success;
    }


    const char* map_lookup_text(hashmap* map, const char* eng)
    {
        if (lang == "en" && !test_mode)
        {
            return eng;
        }

        uintptr_t ptr_tra;
        bool found = hashmap_get(map, (void*) eng, SDL_strlen(eng), &ptr_tra);
        const char* tra = (const char*) ptr_tra;

        if (!found || tra == NULL || tra[0] == '\0')
        {
            if (test_mode)
            {
                return map_store_404(map, eng);
            }
            return eng;
        }

        return tra;
    }

    std::string gettext(const std::string& eng)
    {
        // TODO: take and return const char*
        return std::string(map_lookup_text(map_translation, eng.c_str()));
    }

    std::string gettext_cutscene(const std::string& script_id, const std::string& eng)
    {
        if (lang == "en" && !test_mode)
        {
            return eng;
        }

        uintptr_t ptr_cutscene_map;
        bool found = hashmap_get(map_translation_cutscene, (void*) script_id.c_str(), script_id.size(), &ptr_cutscene_map);
        hashmap* cutscene_map = (hashmap*) ptr_cutscene_map;

        if (!found || cutscene_map == NULL)
        {
            return eng;
        }

        return std::string(map_lookup_text(cutscene_map, eng.c_str()));
    }

    std::string getnumber(int n)
    {
        if (n < 0)
        {
            return help.String(n);
        }
        int ix = n;
        if (n >= 101)
        {
            ix = 101; // Lots
        }

        if (number[ix].empty())
        {
            return help.String(n);
        }
        return number[ix];
    }

    const char* get_roomname_explanation(int roomx, int roomy)
    {
        /* Never returns NULL. */

        if (!fix_room_coords(&roomx, &roomy))
        {
            return "";
        }

        const char* explanation = explanation_roomnames[roomy][roomx];
        if (explanation == NULL)
        {
            return "";
        }
        return explanation;
    }

    const char* get_roomname_translation(int roomx, int roomy)
    {
        /* Only looks for the translation, doesn't return English fallback.
         * Never returns NULL.
         * Also used for room name translation mode. */

        if (!fix_room_coords(&roomx, &roomy))
        {
            return "";
        }

        const char* tra = translation_roomnames[roomy][roomx];
        if (tra == NULL)
        {
            return "";
        }
        return tra;
    }

    const char* gettext_roomname(int roomx, int roomy, const char* eng, bool special)
    {
        if (lang == "en")
        {
            return eng;
        }

        if (special)
        {
            return gettext_roomname_special(eng);
        }

        const char* tra = get_roomname_translation(roomx, roomy);
        if (tra[0] == '\0')
        {
            return eng;
        }
        return tra;
    }

    const char* gettext_roomname_special(const char* eng)
    {
        return map_lookup_text(map_translation_roomnames_special, eng);
    }

    bool is_cutscene_translated(const std::string& script_id)
    {
        if (lang == "en")
        {
            return false;
        }

        uintptr_t ptr_unused;
        return hashmap_get(map_translation_cutscene, (void*) script_id.c_str(), script_id.size(), &ptr_unused);
    }

    uint32_t toupper(uint32_t ch)
    {
        // Convert a single Unicode codepoint to its uppercase variant
        // Supports important Latin (1 and A), Cyrillic and Greek

        // Turkish i?
        if (langmeta.toupper_i_dot && ch == 'i') return 0x130;

        // a-z?
        if ('a' <= ch && ch <= 'z') return ch - 0x20;

        // Latin-1 Supplement? But not the division sign
        if (0xE0 <= ch && ch <= 0xFE && ch != 0xF7) return ch - 0x20;

        // ß? Yes, we do have this! And otherwise we could only replace it with SS later on.
        if (ch == 0xDF) return 0x1E9E;

        // ÿ?
        if (ch == 0xFF) return 0x178;

        // Let's get some exceptions for Latin Extended-A out of the way, starting with ı
        if (ch == 0x131) return 'I';

        // This range between two obscure exceptions...
        if (0x139 <= ch && ch <= 0x148 && ch % 2 == 0) return ch - 1;

        // The rest of Latin Extended-A?
        if (0x100 <= ch && ch <= 0x177 && ch % 2 == 1) return ch - 1;

        // Okay, Ÿ also pushed some aside...
        if (0x179 <= ch && ch <= 0x17E && ch % 2 == 0) return ch - 1;

        // Can't hurt to support Romanian properly...
        if (ch == 0x219 || ch == 0x21B) return ch - 1;

        // Cyrillic а-я?
        if (0x430 <= ch && ch <= 0x44F) return ch - 0x20;

        // There's probably a good reason Cyrillic upper and lower accents are wrapped around the alphabet...
        if (0x450 <= ch && ch <= 0x45F) return ch - 0x50;

        // Apparently a Ukranian letter is all the way over there, why not.
        if (ch == 0x491) return ch - 1;

        // Time for Greek, thankfully we're not making a lowercasing function with that double sigma!
        if (ch == 0x3C2) return 0x3A3;

        // The entire Greek alphabet then, along with two accented letters
        if (0x3B1 <= ch && ch <= 0x3CB) return ch - 0x20;

        // Unfortunately Greek accented letters are all over the place.
        if (ch == 0x3AC) return 0x386;
        if (0x3AD <= ch && ch <= 0x3AF) return ch - 0x25;
        if (ch == 0x3CC) return 0x38C;
        if (ch == 0x3CD || ch == 0x3CE) return ch - 0x3F;

        // Nothing matched! Just leave it as is
        return ch;
    }

    std::string toupper(const std::string& lower)
    {
        // Convert a UTF-8 string to uppercase
        if (!langmeta.toupper)
            return lower;

        std::string upper = std::string();
        std::back_insert_iterator<std::string> inserter = std::back_inserter(upper);
        std::string::const_iterator iter = lower.begin();
        bool ignorenext = false;
        uint32_t ch;
        while (iter != lower.end())
        {
            ch = utf8::unchecked::next(iter);

            if (langmeta.toupper_lower_escape_char && ch == '~')
            {
                ignorenext = true;
                continue;
            }

            if (!ignorenext)
            {
                ch = toupper(ch);
            }
            utf8::unchecked::append(ch, inserter);

            ignorenext = false;
        }

        return upper;
    }

    std::string not_toupper(const std::string& _s)
    {
        // No-op, except if langmeta.toupper_lower_escape_char, to remove the ~ escape character
        // To be clear: does not convert to lowercase!
        // (Hence why not_toupper is the best I could come up with for now to prevent anyone thinking it's just a tolower)

        if (!langmeta.toupper_lower_escape_char)
            return _s;

        std::string s = std::string(_s);
        for (signed int i = s.size()-1; i >= 0; i--)
        {
            if (s[i] == '~')
                s.erase(i, 1);
        }
        return s;
    }

#undef MAP_MAX_X
#undef MAP_MAX_Y

}
