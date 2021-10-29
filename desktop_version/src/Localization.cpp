#include "Localization.h"

#include <stdio.h>
#include <tinyxml2.h>
#include <utf8/unchecked.h>

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
    bool show_lang_maint_menu;

    std::map<std::string, std::string> translation;
    std::map<std::string, std::map<std::string, std::string> > translation_cutscenes;
    std::string number[102];

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

    void resettext(void)
    {
        // Reset/Initialize strings
        translation.clear();
        translation_cutscenes.clear();

        for (size_t i = 0; i <= 101; i++)
        {
            number[i] = "";
        }
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
                std::string eng = std::string(pElem->Attribute("english"));
                if (translation.count(eng) != 0)
                {
                    vlog_warn("\"%s\" appears in language file multiple times", eng.c_str());
                }
                translation[eng] = std::string(pText);
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
                std::string script_id = std::string(pElem->Attribute("id"));
                if (translation_cutscenes.count(script_id) != 0)
                {
                    vlog_warn("Cutscene \"%s\" appears in language file multiple times", script_id.c_str());
                }
                translation_cutscenes[script_id] = std::map<std::string, std::string>();

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
                        std::string eng = std::string(subElem->Attribute("english"));
                        translation_cutscenes[script_id][eng] = std::string(pSubText);
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

    void loadtext(void)
    {
        show_lang_maint_menu = FILESYSTEM_langsAreModded();

        resettext();
        loadmeta(langmeta);

        if (lang == "en" && !test_mode)
        {
            return;
        }

        loadtext_strings();
        loadtext_cutscenes();
        loadtext_numbers();
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
                pElem->SetText(translation[std::string(eng)].c_str());
            }
        }

        FILESYSTEM_saveTiXml2Document(("lang/" + langcode + "/strings.xml").c_str(), doc);
    }

    void sync_lang_files(void)
    {
        std::string oldlang = lang;

        for (size_t i = 0; i < languagelist.size(); i++)
        {
            if (languagelist[i].code != "en")
                sync_lang_file(languagelist[i].code);
        }

        lang = oldlang;
        loadtext();
    }


    std::string gettext(const std::string& eng)
    {
        if (test_mode)
        {
            if (translation.count(eng) != 0)
            {
                return "V";
            }
            return "X";
        }

        if (lang == "en" || translation.count(eng) == 0)
        {
            return eng;
        }

        std::string& tra = translation[eng];

        if (tra.empty())
        {
            return eng;
        }

        return tra;
    }

    std::string gettext_cutscene(const std::string& script_id, const std::string& eng)
    {
        if (!is_cutscene_translated(script_id) || translation_cutscenes[script_id].count(eng) == 0)
        {
            return eng;
        }

        std::string& tra = translation_cutscenes[script_id][eng];

        if (tra.empty())
        {
            return eng;
        }

        return tra;
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

    bool is_cutscene_translated(const std::string& script_id)
    {
        return lang != "en" && translation_cutscenes.count(script_id) != 0;
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
}
