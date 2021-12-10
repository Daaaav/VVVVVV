#define LOCALIZATIONSTORAGE_CPP
#include "Localization.h"
#include "LocalizationStorage.h"

#include <tinyxml2.h>

#include "FileSystemUtils.h"
#include "UtilityClass.h"
#include "Vlogging.h"

#define FOR_EACH_XML_ELEMENT(doc, elem) \
    for ( \
        elem = doc \
            .FirstChildElement() \
            .FirstChildElement() \
            .ToElement(); \
        elem != NULL; \
        elem = elem->NextSiblingElement() \
    )

#define FOR_EACH_XML_SUB_ELEMENT(elem, subelem) \
    for ( \
        subelem = elem->FirstChildElement(); \
        subelem != NULL; \
        subelem = subelem->NextSiblingElement() \
    )

namespace loc
{
    bool inited = false;

    bool load_lang_doc(const std::string& cat, tinyxml2::XMLDocument& doc, const std::string& langcode = lang)
    {
        if (!FILESYSTEM_loadTiXml2Document(("lang/" + langcode + "/" + cat + ".xml").c_str(), doc))
        {
            vlog_info("Could not load language file %s/%s.", langcode.c_str(), cat.c_str());
            return false;
        }
        if (doc.Error())
        {
            vlog_error("Error parsing language file %s/%s: %s", langcode.c_str(), cat.c_str(), doc.ErrorStr());
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
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("meta", doc, langcode))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText();
            if (pText == NULL)
            {
                pText = "";
            }

            if (SDL_strcmp(pKey, "active") == 0)
                meta.active = help.Int(pText);
            else if (SDL_strcmp(pKey, "nativename") == 0)
                meta.nativename = std::string(pText);
            else if (SDL_strcmp(pKey, "credit") == 0)
                meta.credit = std::string(pText);
            else if (SDL_strcmp(pKey, "autowordwrap") == 0)
                meta.autowordwrap = help.Int(pText);
            else if (SDL_strcmp(pKey, "toupper") == 0)
                meta.toupper = help.Int(pText);
            else if (SDL_strcmp(pKey, "toupper_i_dot") == 0)
                meta.toupper_i_dot = help.Int(pText);
            else if (SDL_strcmp(pKey, "toupper_lower_escape_char") == 0)
                meta.toupper_lower_escape_char = help.Int(pText);
        }
    }

    void map_store_translation(Textbook* textbook, hashmap* map, const char* eng, const char* tra)
    {
        if (eng == NULL)
        {
            return;
        }
        if (tra == NULL)
        {
            tra = "";
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
        const char* tb_eng = textbook_store(&textbook_main, eng);
        const char* tb_tra = textbook_store(&textbook_main, ("❌" + std::string(eng)).c_str());

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
            hashmap_free(map_translation_plural);
            hashmap_free(map_translation_roomnames_special);

            textbook_clear(&textbook_main);
        }
        inited = true;

        textbook_init(&textbook_main);

        map_translation = hashmap_create();
        map_translation_cutscene = hashmap_create();
        map_translation_plural = hashmap_create();

        for (size_t i = 0; i <= 101; i++)
        {
            number[i] = "";
        }
        SDL_zeroa(number_plural_form);

        SDL_zeroa(translation_roomnames);
        SDL_zeroa(explanation_roomnames);

        n_untranslated_roomnames = 0;
        n_unexplained_roomnames = 0;

        map_translation_roomnames_special = hashmap_create();
    }

    void loadtext_strings(void)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("strings", doc))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "string") == 0)
            {
                map_store_translation(
                    &textbook_main,
                    map_translation,
                    pElem->Attribute("english"),
                    pElem->Attribute("translation")
                );
            }
        }
    }

    void loadtext_strings_plural(void)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("strings_plural", doc))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "string") == 0)
            {
                const char* eng_plural = pElem->Attribute("english_plural");
                if (eng_plural == NULL)
                {
                    continue;
                }

                tinyxml2::XMLElement* subElem;
                FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
                {
                    const char* pSubKey = subElem->Value();

                    if (SDL_strcmp(pSubKey, "translation") == 0)
                    {
                        size_t alloc_len = 1+SDL_strlen(eng_plural)+1;

                        char* key = (char*) SDL_malloc(alloc_len);
                        char form = subElem->IntAttribute("form", 0);
                        key[0] = form+1;
                        SDL_memcpy(&key[1], eng_plural, alloc_len-1);

                        map_store_translation(
                            &textbook_main,
                            map_translation_plural,
                            key,
                            subElem->Attribute("translation")
                        );

                        SDL_free(key);
                    }
                }
            }
        }
    }

    void loadtext_cutscenes(void)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("cutscenes", doc))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "cutscene") == 0)
            {
                const char* script_id = textbook_store(&textbook_main, pElem->Attribute("id"));
                if (script_id == NULL)
                {
                    continue;
                }

                hashmap* cutscene_map = hashmap_create();
                hashmap_set_free(
                    map_translation_cutscene,
                    (void*) script_id,
                    SDL_strlen(script_id),
                    (uintptr_t) cutscene_map,
                    callback_free_map_value,
                    NULL
                );

                tinyxml2::XMLElement* subElem;
                FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
                {
                    const char* pSubKey = subElem->Value();

                    if (SDL_strcmp(pSubKey, "dialogue") == 0)
                    {
                        map_store_translation(
                            &textbook_main,
                            cutscene_map,
                            subElem->Attribute("english"),
                            subElem->Attribute("translation")
                        );
                    }
                }
            }
        }
    }

    void loadtext_numbers(void)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("numbers", doc))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "number") == 0)
            {
                int value = help.Int(pElem->Attribute("value"));
                if (value >= 0 && value <= 101)
                {
                    const char* tra = pElem->Attribute("translation");
                    if (tra == NULL)
                    {
                        tra = "";
                    }
                    number[value] = std::string(tra);
                    number_plural_form[value] = pElem->IntAttribute("form", 0);
                }
            }
        }
    }

    bool fix_room_coords(int* roomx, int* roomy)
    {
        *roomx %= 100;
        *roomy %= 100;

        if (*roomx == 9 && *roomy == 4)
        {
            // The Tower has two rooms, unify them
            *roomy = 9;
        }

        return !(*roomx < 0 || *roomy < 0 || *roomx > MAP_MAX_X || *roomy > MAP_MAX_Y);
    }

    void update_left_counter(const char* old_text, const char* new_text, int* counter)
    {
        bool now_filled = new_text[0] != '\0';
        if ((old_text == NULL || old_text[0] == '\0') && now_filled)
        {
            (*counter)--;
        }
        else if (old_text != NULL && old_text[0] != '\0' && !now_filled)
        {
            (*counter)++;
        }
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
            update_left_counter(translation_roomnames[roomy][roomx], tra, &n_untranslated_roomnames);
            translation_roomnames[roomy][roomx] = textbook_store(&textbook_main, tra);
        }
        if (explanation != NULL)
        {
            update_left_counter(explanation_roomnames[roomy][roomx], explanation, &n_unexplained_roomnames);
            explanation_roomnames[roomy][roomx] = textbook_store(&textbook_main, explanation);
        }

        return true;
    }

    void loadtext_roomnames(void)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("roomnames", doc))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "roomname") == 0)
            {
                int x = pElem->IntAttribute("x", -1);
                int y = pElem->IntAttribute("y", -1);

                n_untranslated_roomnames++;
                n_unexplained_roomnames++;

                store_roomname_translation(
                    false,
                    x,
                    y,
                    pElem->Attribute("translation"),
                    show_translator_menu ? pElem->Attribute("explanation") : NULL
                );
            }
        }
    }

    void loadtext_roomnames_special(void)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("roomnames_special", doc))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "roomname") == 0)
            {
                map_store_translation(
                    &textbook_main,
                    map_translation_roomnames_special,
                    pElem->Attribute("english"),
                    pElem->Attribute("translation")
                );
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
                n_untranslated_roomnames = 0;
            }

            return;
        }

        loadtext_numbers();
        loadtext_strings();
        loadtext_strings_plural();
        loadtext_cutscenes();
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
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("strings", doc, "en"))
        {
            return;
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
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
                pElem->SetAttribute("translation", tra);
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
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        if (!load_lang_doc("roomnames", doc, langcode))
        {
            return false;
        }

        bool found = false;
        FOR_EACH_XML_ELEMENT(hDoc, pElem)
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
                        pElem->SetAttribute("translation", tra);
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
        bool save_success = FILESYSTEM_saveTiXml2Document((langcode + "/roomnames.xml").c_str(), doc);
        FILESYSTEM_restoreWriteDir();
        if (!save_success)
        {
            vlog_error("Could not write roomnames document!");
            return false;
        }
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
}
