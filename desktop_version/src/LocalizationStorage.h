#ifndef LOCALIZATIONSTORAGE_H
#define LOCALIZATIONSTORAGE_H

#include "Textbook.h"

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

#if defined(LOCALIZATIONSTORAGE_CPP)
    #define LS_INTERN
#else
    #define LS_INTERN extern
#endif

namespace loc
{
    #if defined(LOCALIZATION_CPP) || defined(LOCALIZATIONSTORAGE_CPP)
        LS_INTERN Textbook textbook_main;
        LS_INTERN Textbook textbook_custom;

        LS_INTERN hashmap* map_translation;
        LS_INTERN hashmap* map_translation_plural;
        LS_INTERN std::string number[102]; /* [101] for Lots */
        LS_INTERN unsigned char number_plural_form[200]; /* [0..99] for 0..99, [100..199] for *00..*99 */
        LS_INTERN hashmap* map_translation_cutscene;
        LS_INTERN hashmap* map_translation_cutscene_custom;
        LS_INTERN hashmap* map_translation_roomnames_special;

        #define MAP_MAX_X 54
        #define MAP_MAX_Y 56
        #define CUSTOM_MAP_MAX_X 19
        #define CUSTOM_MAP_MAX_Y 19
        LS_INTERN const char* translation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
        LS_INTERN const char* explanation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
        LS_INTERN const char* translation_roomnames_custom[CUSTOM_MAP_MAX_Y+1][CUSTOM_MAP_MAX_X+1];
        LS_INTERN const char* explanation_roomnames_custom[CUSTOM_MAP_MAX_Y+1][CUSTOM_MAP_MAX_X+1];
    #endif


    void resettext_custom(void);
    void unloadtext_custom(void);

    bool store_roomname_translation(bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);

    bool fix_room_coords(bool custom_level, int* roomx, int* roomy);

    void loadtext(void);
    void loadtext_custom(const char* custom_path);
    void loadlanguagelist(void);
    void sync_lang_files(void);

    bool save_roomname_to_file(const std::string& langcode, bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);
    bool save_roomname_explanation_to_files(bool custom_level, int roomx, int roomy, const char* explanation);

    const char* map_lookup_text(hashmap* map, const char* eng, const char* fallback);

    char* add_disambiguator(char disambiguator, const char* original_string, size_t* ext_alloc_len);
}

#undef LS_INTERN

#endif /* LOCALIZATIONSTORAGE_H */
