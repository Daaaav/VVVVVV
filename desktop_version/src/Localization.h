#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <stdint.h>
#include <string>
#include <vector>

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

    struct TextboxFormat
    {
        const char* text;
        unsigned short wraplimit; // = 36*8-pad_left-pad_right; no effect if tt or !langmeta.autowordwrap
        bool tt; // teletype, don't auto-wordwrap
        bool centertext; // whether the text should be centered inside the box
        unsigned char pad_left; // pad with X characters
        unsigned char pad_right;
        unsigned short padtowidth; // pad to X pixels (0 to disable)
    };

    extern std::string lang;
    extern LangMeta langmeta;
    extern bool test_mode;
    extern std::vector<LangMeta> languagelist;
    extern int languagelist_curlang;
    extern bool show_translator_menu;

    extern int n_untranslated_roomnames;
    extern int n_unexplained_roomnames;


    const char* gettext(const char* eng);
    const char* gettext_plural(const char* eng_plural, const char* eng_singular, int count);
    void gettext_plural_fill(char* buf, size_t buf_len, const char* eng_plural, const char* eng_singular, int count);
    std::string getnumber(int n);
    const TextboxFormat* gettext_cutscene(const std::string& script_id, const std::string& eng, char textcase);
    const char* get_roomname_explanation(int roomx, int roomy);
    const char* get_roomname_translation(int roomx, int roomy);
    const char* gettext_roomname(int roomx, int roomy, const char* eng, bool special);
    const char* gettext_roomname_special(const char* eng);

    bool is_cutscene_translated(const std::string& script_id);

    std::string toupper(const std::string& lower);
    std::string not_toupper(const std::string& _s);
}

#endif /* LOCALIZATION_H */
