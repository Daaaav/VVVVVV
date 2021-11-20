#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

extern "C"
{
    /* <map.h> */
    typedef struct hashmap hashmap;
    typedef void (*hashmap_callback)(void *key, size_t ksize, uintptr_t value, void *usr);

    hashmap* hashmap_create(void);
    void hashmap_free(hashmap* map);
    void hashmap_set(hashmap* map, void* key, size_t ksize, uintptr_t value);
    void hashmap_set_free(hashmap* map, void* key, size_t ksize, uintptr_t value, hashmap_callback c, void* usr);
    bool hashmap_get(hashmap* map, void* key, size_t ksize, uintptr_t* out_val);
    void hashmap_iterate(hashmap* map, hashmap_callback c, void* usr);
}

namespace loc
{
    struct LangMeta
    {
        bool active; // = true, language is shown in the list
        std::string code;
        std::string nativename;
        std::string credit;
        bool autowordwrap; // = true; enable automatic wordwrapping
        bool toupper; // = true; enable automatic full-caps for menu options
        bool toupper_i_dot; // = false; enable Turkish i mapping when uppercasing
        bool toupper_lower_escape_char; // = false; enable ~ to mark lowercase letters for uppercasing
    };

    /* The purpose of a Textbook is to store, potentially, a lot of text on a pile that shouldn't
     * go anywhere until we change languages or (for example) unload an entire level's text. */
#define TEXTBOOK_MAX_PAGES 1000
#define TEXTBOOK_PAGE_SIZE 50000
    struct Textbook
    {
        char* page[TEXTBOOK_MAX_PAGES];
        size_t page_len[TEXTBOOK_MAX_PAGES];

        short pages_used;
    };

    extern std::string lang;
    extern LangMeta langmeta;
    extern bool test_mode;
    extern std::vector<LangMeta> languagelist;
    extern int languagelist_curlang;
    extern bool show_translator_menu;

    extern int n_untranslated_roomnames;
    extern int n_unexplained_roomnames;

    bool store_roomname_translation(bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);

    void loadtext(void);
    void loadlanguagelist(void);
    void sync_lang_files(void);
    bool save_roomname_to_file(const std::string& langcode, bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);
    bool save_roomname_explanation_to_files(bool custom_level, int roomx, int roomy, const char* explanation);

    std::string gettext(const std::string& eng);
    std::string gettext_cutscene(const std::string& script_id, const std::string& eng);
    std::string getnumber(int n);
    const char* get_roomname_explanation(int roomx, int roomy);
    const char* get_roomname_translation(int roomx, int roomy);
    const char* gettext_roomname(int roomx, int roomy, const char* eng, bool special);
    const char* gettext_roomname_special(const char* eng);

    bool is_cutscene_translated(const std::string& script_id);

    std::string toupper(const std::string& lower);
    std::string not_toupper(const std::string& _s);
}

#endif /* LOCALIZATION_H */
