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

        LS_INTERN hashmap* map_translation;
        LS_INTERN hashmap* map_translation_plural;
        LS_INTERN std::string number[102];
        LS_INTERN char number_plural_form[102];
        LS_INTERN hashmap* map_translation_cutscene;
        LS_INTERN hashmap* map_translation_roomnames_special;

        #define MAP_MAX_X 54
        #define MAP_MAX_Y 56
        LS_INTERN const char* translation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
        LS_INTERN const char* explanation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
    #endif


    bool store_roomname_translation(bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);

    bool fix_room_coords(int* roomx, int* roomy);

    void loadtext(void);
    void loadlanguagelist(void);
    void sync_lang_files(void);

    bool save_roomname_to_file(const std::string& langcode, bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);
    bool save_roomname_explanation_to_files(bool custom_level, int roomx, int roomy, const char* explanation);

    const char* map_lookup_text(hashmap* map, const char* eng);
}

#undef LS_INTERN

#endif /* LOCALIZATIONSTORAGE_H */
