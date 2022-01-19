#define LOCALIZATIONSTORAGE_CPP
#include "Localization.h"
#include "LocalizationStorage.h"

#include <tinyxml2.h>

#include "CustomLevels.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
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
    bool inited_custom = false;

    char* custom_level_path = NULL;

    bool load_lang_doc(
        const std::string& cat,
        tinyxml2::XMLDocument& doc,
        const std::string& langcode = lang,
        const std::string& asset_cat = ""
    )
    {
        /* Load a language-related XML file.
         * cat is the "category", so "strings", "numbers", etc.
         *
         * asset_cat is only used when loading
         * from custom level assets is possible. */

        bool asset_loaded = false;
        if (!asset_cat.empty())
        {
            asset_loaded = FILESYSTEM_loadAssetTiXml2Document(("lang/" + langcode + "/" + asset_cat + ".xml").c_str(), doc);
        }
        if (!asset_loaded && !FILESYSTEM_loadTiXml2Document(("lang/" + langcode + "/" + cat + ".xml").c_str(), doc))
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
            else if (SDL_strcmp(pKey, "action_hint") == 0)
                meta.action_hint = std::string(pText);
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

    void resettext_custom(void)
    {
        // Reset/Initialize custom level strings only
        if (inited_custom)
        {
            hashmap_iterate(map_translation_cutscene_custom, callback_free_map_value, NULL);
            hashmap_free(map_translation_cutscene_custom);

            textbook_clear(&textbook_custom);
        }
        inited_custom = true;

        textbook_init(&textbook_custom);

        map_translation_cutscene_custom = hashmap_create();

        SDL_zeroa(translation_roomnames_custom);
        SDL_zeroa(explanation_roomnames_custom);

        n_untranslated_roomnames_custom = 0;
        n_unexplained_roomnames_custom = 0;
    }

    void unloadtext_custom(void)
    {
        resettext_custom();

        loc::lang_custom = "";

        if (custom_level_path != NULL)
        {
            SDL_free(custom_level_path);
            custom_level_path = NULL;
        }
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

        resettext_custom();
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
                        char form = subElem->IntAttribute("form", 0);
                        char* key = add_disambiguator(form+1, eng_plural, NULL);
                        if (key == NULL)
                        {
                            continue;
                        }

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

    bool get_level_lang_path(bool custom_level, const char* cat, std::string& doc_path, std::string& doc_path_asset)
    {
        /* Calculate the path to a translation file for either the MAIN GAME or
         * a CUSTOM LEVEL. cat can be "roomnames", "cutscenes", etc.
         *
         * doc_path and doc_path_asset are "out" parameters, and will be set to
         * the appropriate filenames to use for language files outside of or
         * inside level assets respectively (translations for custom levels can
         * live in the main language folders too)
         *
         * Returns whether this is a (valid) custom level path. */

        if (custom_level
            && custom_level_path != NULL
            && SDL_strncmp(custom_level_path, "levels/", 7) == 0
            && SDL_strlen(custom_level_path) > (sizeof(".vvvvvv")-1)
        )
        {
            /* Get rid of .vvvvvv */
            size_t len = SDL_strlen(custom_level_path)-7;
            doc_path = std::string(custom_level_path, len);
            doc_path.append("/custom_");
            doc_path.append(cat);

            /* For the asset path, also get rid of the levels/LEVELNAME/ */
            doc_path_asset = "custom_";
            doc_path_asset.append(cat);

            return true;
        }
        else
        {
            doc_path = cat;
            doc_path_asset = "";

            return false;
        }
    }

    const char* get_level_original_lang(tinyxml2::XMLHandle& hDoc)
    {
        /* cutscenes and roomnames files can specify the original language as
         * an attribute of the root tag to change the attribute names of the
         * original text (normally "english"). This makes level translations
         * less confusing if the original language isn't English. */

        const char* original = NULL;
        tinyxml2::XMLElement* pRoot = hDoc.FirstChildElement().ToElement();
        if (pRoot != NULL)
        {
            original = pRoot->Attribute("original");
        }
        if (original == NULL)
        {
            original = "english";
        }
        return original;
    }

    std::string& get_level_lang_code(bool custom_level)
    {
        if (!custom_level || lang_custom == "")
        {
            return lang;
        }

        return lang_custom;
    }

    void loadtext_cutscenes(bool custom_level)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        std::string doc_path;
        std::string doc_path_asset;
        bool valid_custom_level = get_level_lang_path(custom_level, "cutscenes", doc_path, doc_path_asset);
        if (custom_level && !valid_custom_level)
        {
            return;
        }
        if (!load_lang_doc(doc_path, doc, get_level_lang_code(custom_level), doc_path_asset))
        {
            return;
        }

        Textbook* textbook;
        hashmap* map;
        if (custom_level)
        {
            textbook = &textbook_custom;
            map = map_translation_cutscene_custom;
        }
        else
        {
            textbook = &textbook_main;
            map = map_translation_cutscene;
        }

        const char* original = get_level_original_lang(hDoc);

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "cutscene") == 0)
            {
                const char* script_id = textbook_store(textbook, pElem->Attribute("id"));
                if (script_id == NULL)
                {
                    continue;
                }

                hashmap* cutscene_map = hashmap_create();
                hashmap_set_free(
                    map,
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
                        const char* eng = subElem->Attribute(original);
                        const char* tra = subElem->Attribute("translation");
                        if (eng == NULL || tra == NULL)
                        {
                            continue;
                        }
                        const std::string eng_unwrapped = graphics.string_unwordwrap(eng);
                        char* eng_prefixed = add_disambiguator(subElem->UnsignedAttribute("case", 1), eng_unwrapped.c_str(), NULL);
                        if (eng_prefixed == NULL)
                        {
                            continue;
                        }
                        const char* tb_eng = textbook_store(textbook, eng_prefixed);
                        const char* tb_tra = textbook_store(textbook, tra);
                        SDL_free(eng_prefixed);
                        if (tb_eng == NULL || tb_tra == NULL)
                        {
                            continue;
                        }
                        TextboxFormat format;
                        format.text = tb_tra;
                        format.tt = subElem->BoolAttribute("tt", false);
                        format.centertext = subElem->BoolAttribute("centertext", false);
                        format.pad_left = subElem->UnsignedAttribute("pad_left", 0);
                        format.pad_right = subElem->UnsignedAttribute("pad_right", 0);
                        unsigned short pad = subElem->UnsignedAttribute("pad", 0);
                        format.pad_left += pad;
                        format.pad_right += pad;
                        format.wraplimit = subElem->UnsignedAttribute("wraplimit",
                            36*8 - (format.pad_left+format.pad_right)*8
                        );
                        format.padtowidth = subElem->UnsignedAttribute("padtowidth", 0);

                        const TextboxFormat* tb_format = (TextboxFormat*) textbook_store_raw(
                            textbook,
                            &format,
                            sizeof(TextboxFormat)
                        );
                        if (tb_format == NULL)
                        {
                            continue;
                        }
                        hashmap_set(cutscene_map, (void*) tb_eng, SDL_strlen(tb_eng), (uintptr_t) tb_format);
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
                const char* value_str = pElem->Attribute("value");
                bool is_lots = SDL_strcmp(value_str, "lots") == 0;
                int value = help.Int(value_str);
                if (is_lots)
                {
                    value = 101;
                }
                if ((value >= 0 && value <= 100) || is_lots)
                {
                    const char* tra = pElem->Attribute("translation");
                    if (tra == NULL)
                    {
                        tra = "";
                    }
                    number[value] = std::string(tra);
                }
                if (value >= 0 && value <= 199 && !is_lots)
                {
                    int form = pElem->IntAttribute("form", 0);
                    number_plural_form[value] = form;
                    if (value < 100)
                    {
                        number_plural_form[value+100] = form;
                    }
                }
            }
        }
    }

    bool fix_room_coords(bool custom_level, int* roomx, int* roomy)
    {
        *roomx %= 100;
        *roomy %= 100;

        if (!custom_level && *roomx == 9 && *roomy == 4)
        {
            // The Tower has two rooms, unify them
            *roomy = 9;
        }

        int max_x = MAP_MAX_X;
        int max_y = MAP_MAX_Y;
        if (custom_level)
        {
            max_x = CUSTOM_MAP_MAX_X;
            max_y = CUSTOM_MAP_MAX_Y;
        }

        return !(*roomx < 0 || *roomy < 0 || *roomx > max_x || *roomy > max_y);
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
        if (!fix_room_coords(custom_level, &roomx, &roomy))
        {
            return false;
        }

        /* We have some arrays filled with pointers, and we need to change those pointers */
        const char** ptr_translation;
        const char** ptr_explanation;
        int* ptr_n_untranslated;
        int* ptr_n_unexplained;
        if (custom_level)
        {
            ptr_translation = &translation_roomnames_custom[roomy][roomx];
            ptr_explanation = &explanation_roomnames_custom[roomy][roomx];
            ptr_n_untranslated = &n_untranslated_roomnames_custom;
            ptr_n_unexplained = &n_unexplained_roomnames_custom;
        }
        else
        {
            ptr_translation = &translation_roomnames[roomy][roomx];
            ptr_explanation = &explanation_roomnames[roomy][roomx];
            ptr_n_untranslated = &n_untranslated_roomnames;
            ptr_n_unexplained = &n_unexplained_roomnames;
        }

        if (tra != NULL)
        {
            update_left_counter(*ptr_translation, tra, ptr_n_untranslated);
            *ptr_translation = textbook_store(&textbook_main, tra);
        }
        if (explanation != NULL)
        {
            update_left_counter(*ptr_explanation, explanation, ptr_n_unexplained);
            *ptr_explanation = textbook_store(&textbook_main, explanation);
        }

        return true;
    }

    void loadtext_roomnames(bool custom_level)
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;

        std::string doc_path;
        std::string doc_path_asset;
        bool valid_custom_level = get_level_lang_path(custom_level, "roomnames", doc_path, doc_path_asset);
        if (custom_level && !valid_custom_level)
        {
            return;
        }
        if (!load_lang_doc(doc_path, doc, get_level_lang_code(custom_level), doc_path_asset))
        {
            return;
        }

        const char* original = get_level_original_lang(hDoc);

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "roomname") == 0)
            {
                int x = pElem->IntAttribute("x", -1);
                int y = pElem->IntAttribute("y", -1);

                if (custom_level)
                {
                    /* Extra safeguard: make sure the original room name matches! */
                    const char* original_roomname = pElem->Attribute(original);
                    if (original_roomname == NULL)
                    {
                        continue;
                    }
                    #if !defined(NO_CUSTOM_LEVELS)
                        const RoomProperty* const room = cl.getroomprop(x, y);
                        if (SDL_strcmp(original_roomname, room->roomname.c_str()) != 0)
                        {
                            continue;
                        }
                    #else
                        continue;
                    #endif

                    n_untranslated_roomnames_custom++;
                    n_unexplained_roomnames_custom++;
                }
                else
                {
                    n_untranslated_roomnames++;
                    n_unexplained_roomnames++;
                }

                store_roomname_translation(
                    custom_level,
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

    void loadtext_custom(const char* custom_path)
    {
        resettext_custom();
        if (custom_level_path == NULL && custom_path != NULL)
        {
            custom_level_path = SDL_strdup(custom_path);
        }
        loadtext_cutscenes(true);
        loadtext_roomnames(true);
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
                loadtext_roomnames(false);
                n_untranslated_roomnames = 0;
            }
        }
        else
        {
            loadtext_numbers();
            loadtext_strings();
            loadtext_strings_plural();
            loadtext_cutscenes(false);
            loadtext_roomnames(false);
            loadtext_roomnames_special();
        }

        if (custom_level_path != NULL)
        {
            loadtext_custom(NULL);
        }
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
            vlog_error("Saving custom level room names not implemented");
            return false;
        }

        if (!fix_room_coords(custom_level, &roomx, &roomy))
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

    char* add_disambiguator(char disambiguator, const char* original_string, size_t* ext_alloc_len)
    {
        /* Create a version of the string prefixed with the given byte.
         * This byte is used when the English string is just not enough to identify the correct translation.
         * It's needed to store plural forms, and when the same text appears multiple times in a cutscene.
         * Caller must SDL_free. */

        size_t alloc_len = 1+SDL_strlen(original_string)+1;

        char* alloc = (char*) SDL_malloc(alloc_len);
        if (alloc == NULL)
        {
            return NULL;
        }
        alloc[0] = disambiguator;
        SDL_memcpy(&alloc[1], original_string, alloc_len-1);

        if (ext_alloc_len != NULL)
        {
            *ext_alloc_len = alloc_len;
        }

        return alloc;
    }
}
