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

    const char* gettext(const char* eng);
    const char* gettext_plural(const char* eng_plural, const char* eng_singular, int count);
    void gettext_plural_fill(char* buf, size_t buf_len, const char* eng_plural, const char* eng_singular, int count);
    std::string getnumber(int n);
    std::string gettext_cutscene(const std::string& script_id, const std::string& eng);
    const char* get_roomname_explanation(int roomx, int roomy);
    const char* get_roomname_translation(int roomx, int roomy);
    const char* gettext_roomname(int roomx, int roomy, const char* eng, bool special);
    const char* gettext_roomname_special(const char* eng);

    bool is_cutscene_translated(const std::string& script_id);

    std::string toupper(const std::string& lower);
    std::string not_toupper(const std::string& _s);
}

#endif /* LOCALIZATION_H */
