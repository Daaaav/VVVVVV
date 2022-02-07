#define LOCALIZATION_CPP
#include "Localization.h"
#include "LocalizationStorage.h"

#include <utf8/unchecked.h>

#include "Game.h"
#include "UtilityClass.h"

namespace loc
{
    bool lang_set = false;
    std::string lang = "en";
    std::string lang_custom = "";
    LangMeta langmeta;

    // language screen list
    std::vector<LangMeta> languagelist;
    int languagelist_curlang;
    bool show_translator_menu;
    size_t limitscheck_current_overflow;

    int n_untranslated_roomnames = 0;
    int n_unexplained_roomnames = 0;
    int n_untranslated_roomnames_custom = 0;
    int n_unexplained_roomnames_custom = 0;

    LangMeta* get_langmeta(void)
    {
        if (game.currentmenuname == Menu::language && (unsigned)game.currentmenuoption < languagelist.size())
        {
            return &languagelist[game.currentmenuoption];
        }

        return &langmeta;
    }

    const char* gettext(const char* eng)
    {
        if (lang == "en")
        {
            return eng;
        }

        return map_lookup_text(map_translation, eng, eng);
    }

    const char* gettext_plural_english(const char* eng_plural, const char* eng_singular, int n)
    {
        /* Do be consistent with negative number handling for other languages... */
        if (n == 1 || n == -1)
        {
            return eng_singular;
        }
        return eng_plural;
    }

    const char* gettext_plural(const char* eng_plural, const char* eng_singular, int n)
    {
        if (lang != "en")
        {
            char form = form_for_count(n);
            char* key = add_disambiguator(form+1, eng_plural, NULL);
            if (key != NULL)
            {
                const char* tra = map_lookup_text(map_translation_plural, key, NULL);

                SDL_free(key);

                if (tra != NULL)
                {
                    return tra;
                }
            }
        }
        return gettext_plural_english(eng_plural, eng_singular, n);
    }

    void gettext_plural_fill(char* buf, size_t buf_len, const char* eng_plural, const char* eng_singular, int count)
    {
        const char* tra = gettext_plural(eng_plural, eng_singular, count);
        if (SDL_strstr(tra, "%d") != NULL)
        {
            SDL_snprintf(buf, buf_len, tra, count);
        }
        else
        {
            SDL_snprintf(buf, buf_len, tra, help.number_words(count).c_str());
        }
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

    bool is_script_custom(const char* script_id)
    {
        return SDL_strncmp(script_id, "custom_", 7) == 0;
    }

    const TextboxFormat* gettext_cutscene(const std::string& script_id, const std::string& eng, char textcase)
    {
        hashmap* map;
        const char* map_script_key;
        if (is_script_custom(script_id.c_str()))
        {
            map = map_translation_cutscene_custom;
            map_script_key = &script_id.c_str()[7];
        }
        else
        {
            if (lang == "en")
            {
                return NULL;
            }

            map = map_translation_cutscene;
            map_script_key = script_id.c_str();
        }

        uintptr_t ptr_cutscene_map;
        bool found = hashmap_get(map, (void*) map_script_key, SDL_strlen(map_script_key), &ptr_cutscene_map);
        hashmap* cutscene_map = (hashmap*) ptr_cutscene_map;

        if (!found || cutscene_map == NULL)
        {
            return NULL;
        }

        size_t alloc_len;
        char* key = add_disambiguator(textcase, eng.c_str(), &alloc_len);
        if (key == NULL)
        {
            return NULL;
        }

        uintptr_t ptr_format;
        found = hashmap_get(cutscene_map, (void*) key, alloc_len-1, &ptr_format);
        const TextboxFormat* format = (TextboxFormat*) ptr_format;

        SDL_free(key);

        if (!found)
        {
            return NULL;
        }
        return format;
    }

    const char* get_roomname_explanation(bool custom_level, int roomx, int roomy)
    {
        /* Never returns NULL. */

        if (!fix_room_coords(custom_level, &roomx, &roomy))
        {
            return "";
        }

        const char* explanation;
        if (custom_level)
        {
            explanation = explanation_roomnames_custom[roomy][roomx];
        }
        else
        {
            explanation = explanation_roomnames[roomy][roomx];
        }
        if (explanation == NULL)
        {
            return "";
        }
        return explanation;
    }

    const char* get_roomname_translation(bool custom_level, int roomx, int roomy)
    {
        /* Only looks for the translation, doesn't return English fallback.
         * Never returns NULL.
         * Also used for room name translation mode. */

        if (!fix_room_coords(custom_level, &roomx, &roomy))
        {
            return "";
        }

        const char* tra;
        if (custom_level)
        {
            tra = translation_roomnames_custom[roomy][roomx];
        }
        else
        {
            tra = translation_roomnames[roomy][roomx];
        }

        if (tra == NULL)
        {
            return "";
        }
        return tra;
    }

    const char* gettext_roomname(bool custom_level, int roomx, int roomy, const char* eng, bool special)
    {
        if (!custom_level && lang == "en")
        {
            return eng;
        }

        if (special)
        {
            return gettext_roomname_special(eng);
        }

        const char* tra = get_roomname_translation(custom_level, roomx, roomy);
        if (tra[0] == '\0')
        {
            return eng;
        }
        return tra;
    }

    const char* gettext_roomname_special(const char* eng)
    {
        if (lang == "en")
        {
            return eng;
        }

        return map_lookup_text(map_translation_roomnames_special, eng, eng);
    }

    bool is_cutscene_translated(const std::string& script_id)
    {
        hashmap* map;
        const char* map_script_key;
        if (is_script_custom(script_id.c_str()))
        {
            map = map_translation_cutscene_custom;
            map_script_key = &script_id.c_str()[7];
        }
        else
        {
            if (lang == "en")
            {
                return false;
            }

            map = map_translation_cutscene;
            map_script_key = script_id.c_str();
        }

        uintptr_t ptr_unused;
        return hashmap_get(map, (void*) map_script_key, SDL_strlen(map_script_key), &ptr_unused);
    }

    uint32_t toupper(uint32_t ch)
    {
        // Convert a single Unicode codepoint to its uppercase variant
        // Supports important Latin (1 and A), Cyrillic and Greek

        // Turkish i?
        if (get_langmeta()->toupper_i_dot && ch == 'i') return 0x130;

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
        if (!get_langmeta()->toupper)
            return lower;

        std::string upper = std::string();
        std::back_insert_iterator<std::string> inserter = std::back_inserter(upper);
        std::string::const_iterator iter = lower.begin();
        bool ignorenext = false;
        uint32_t ch;
        while (iter != lower.end())
        {
            ch = utf8::unchecked::next(iter);

            if (get_langmeta()->toupper_lower_escape_char && ch == '~')
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

        if (!get_langmeta()->toupper_lower_escape_char)
            return _s;

        std::string s = std::string(_s);
        for (signed int i = s.size()-1; i >= 0; i--)
        {
            if (s[i] == '~')
                s.erase(i, 1);
        }
        return s;
    }

}
