=== I N T R O D U C T I O N ===

This file will explain everything you need to know when making translations of VVVVVV, or maintaining them.

This file is encoded in UTF-8.



=== A D D I N G   A   N E W   L A N G U A G E ===

The English language files are basically templates for other languages (all the translations are empty).

To create a new language, simply copy the `en` folder, and start by filling out meta.xml (further explained below).



=== T R A N S L A T O R   M E N U ===

The translator menu has options for both translators and maintainers - it allows testing menus, translating room names within the game, syncing all languages with the English template file, and getting statistics on translation progress.

VVVVVV will show a "translator" menu in the main menu if either:
- The "lang" folder is NOT next to data.zip, and the game is running somewhere within a "desktop_version" folder, and desktop_version/lang IS found. This normally happens when compiling the game from source;
- The command line argument (or launch option) "-translator" is passed.

For maintainers: To add new strings, add them to the English strings.xml, and use the option to sync all languages from the translator menu. This will add the new strings to all translated language files.



=== L O W E R C A S E   A N D   U P P E R C A S E ===

If lowercase and uppercase does not exist in your language (Chinese, Japanese and Korean for example), you can set toupper to 0 in meta.xml, and ignore any directions about using lowercase or uppercase.

VVVVVV's menu system has the style of using lowercase for unselected options and uppercase for selected options, for example:

  play
    levels
    [ OPTIONS ]
        translator
          credits
            quit

The menu options are stored as their full-lowercase version, and they're normally commented as "menu option" in the translation files. A built-in function (toupper in Localization.cpp) automatically converts the lowercase text to uppercase when needed. This function has support for a good number of accented characters, Cyrillic and Greek, but more could be added if needed. It also accounts for special cases in Turkish and Irish.

Turkish: The uppercase of i is İ, for example, "dil" becomes "DİL" and not "DIL". To enable this, set toupper_i_dot to 1 in meta.xml.

Irish: Specific letters may be kept in lowercase when making a string full-caps. For example, "mac tíre na hainnise" should be "MAC TÍRE NA hAINNISE" instead of "MAC TÍRE NA HAINNISE". If enabled, you can use the ~ character before the letter which should be forced in lowercase: "mac tíre na ~hainnise". This can be enabled by setting toupper_lower_escape_char to 1 in meta.xml.



=== W O R D W R A P P I N G   A N D   L E N G T H   L I M I T S ===

For most languages, VVVVVV can automatically wordwrap based on spaces. This may not work for some languages (like Chinese, Japanese and Korean), so... TODO

VVVVVV's resolution is 320x240, and the default font is 8x8, which means there is a 40x30 character grid (although we don't adhere to this grid for the UI, but it gives a good indication). Naturally, if the font has a different size like 12x12, less characters will fit on the screen too.

Strings are usually annotated with their limits (for example, max="38*3"). This can be formatted like one of the following:
  (A) 33
  (B) 33*3
  (C) 22 @12x12
  (D) 22*2 @12x12

(A) if it's a single number (for example "33"): the hard maximum number of characters that are known to fit, assuming the 8x8 font. Being exactly on the limit may not look good, so try to go at least a character under it if possible.

(B) if X*Y (for example 33*3): the text should fit within an area of X characters wide and Y lines high, assuming the 8x8 font. The text is automatically word-wrapped to fit (unless disabled in meta.xml). If automatic word-wrapping is disabled, you need to manually insert newlines with |, or possibly as a literal newline (may be &#10; in XML).

(C/D) if it looks like "22 @12x12" or "22*2 @12x12", this means the same as the options above, but the limits have been adapted to the size of the font (12x12 in this example). To get this notation, either use the maintenance option to sync language files from within VVVVVV, or use the Excel document. Ensure the correct font is set in meta.xml first.

The maximum lengths are not always given. Notoriously, menu option buttons are placed diagonally, thus they have maximums that are hard to look up. Even more so, making an option differ too much in length from the other options might even make it look out of place. Best thing to do there is probably just translate as usual and then test all menus. However, menus do automatically reposition based on the text length, so worst-case scenario, if an option is 36 characters long, all options are displayed right underneath each other.

TODO: rename autowordwrap to manual_wordwrap, and invert how it works (and allow | for manual linewraps)



=== F O N T S ===

The game uses an 8x8 pixel font by default (font.png and font.txt in the "fonts" folder). If your language can be represented in 8x8 characters, it is prefable to use this font. TODO

TODO: The fonts directory will also have a README.txt file



=== N U M B E R S   A N D   P L U R A L   F O R M S ===

In certain places, VVVVVV (perhaps unconventionally) writes out numbers as full words. For example:

  - One out of Fifteen
  - Two crewmates remaining
  - Two remaining

These words can be found in numbers.xml. The numbers Zero through Twenty will be the most commonly seen. It's always possible for numbers up to One Hundred to be seen though (players can put up to 100 trinkets and crewmates in a custom level).

"Lots" is used for any number above 100, but this will only show up in unusual/glitchy situations where the user is basically asking for it anyway, so it doesn't have to fit correctly in all these examples (and it already forms questionable sentences in English).

Your language may not allow the same word to be used in these different scenarios. For example, in Polish, "twenty out of twenty" may be "dwadzieścia z dwudziestu". It's possible to leave the translations for all the numbers empty. In that case, numeric forms will be used automatically (20 out of 20).

In English, using Title Case is appropriate, but in most other languages, it probably isn't. Therefore, you may want to translate all numbers in lowercase, when it's more appropriate to use "twenty out of twenty" than "Twenty out of Twenty".

English and some other languages have a singular (1 crewmate) and a plural (2 crewmates). Other languages may work differently than that and have more possible forms depending on the number. These different plural forms can be defined in numbers.xml, by giving them a number that identify that form uniquely. For English, form 1 is used for singular, and form 0 is used for plural. You can set up any amount of plural forms you will need.

Numbers that identify the forms do not need to be sequential, you may use any number between 0 and 254 to identify the different forms. So instead of using forms 0, 1, 2 and 3, you could also name them 1, 2, 5 and 7.

When you have decided on the different forms, you can use them when translating strings_plural.

Suppose you need a different form for the number 1, the numbers 2-4, and all other numbers. You could use "form 1" for the number 1, "form 2" for 2-4, and "form 0" for all other numbers:

<numbers>
    <number value="0"  form="0"  ... />
    <number value="1"  form="1"  ... />
    <number value="2"  form="2"  ... />
    <number value="3"  form="2"  ... />
    <number value="4"  form="2"  ... />
    <number value="5"  form="0"  ... />
    <number value="6"  form="0"  ... />
    ...

When translating the plural strings, you can add translations for every unique form. For example:

    <string english_plural="You rescued %s crewmates" english_singular="You rescued %s crewmate">
        <translation form="0" translation="You saved %s crewmates"/>
        <translation form="1" translation="You saved %s crewmate"/>
        <translation form="2" translation="You saved %s crewmateys"/>
    </string>



=== S T O R Y   A N D   C H A R A C T E R   I N F O R M A T I O N ===

TODO: basic story information, crewmate names, ranks (Captain/Doctor/Professor/Officer/Chief), genders/pronouns (what if you're forced to specify Viridian's gender in another language), personalities, level of (in)formality between crewmates, relationships, who wrote these "personal logs" you can find on terminals...



=== E X C E L ===

The game uses XML files for storing the translations. If you prefer, there is an .xlsm file which can be used as an editor. This can load in all the XML files, and then save changes back as XML. TODO



=== F I L E S ===

== meta.xml ==
This file contains some general information about this translation. It contains the following attributes:

* active: If 0, this language will not be shown in the languages menu

* nativename: The name of the language in itself, fully in lowercase (so not "spanish" or "Español", but "español")

* credit: You can fill in credit here that will appear on the language screen, like "Spanish translation by X". May be in your language.

* autowordwrap: Whether automatic wordwrapping is enabled. Can be disabled for CJK (in which case newlines have to be inserted manually in text)

* toupper: Whether to enable automatic uppercasing of menu options (unselected, SELECTED). May be disabled for languages such as CJK that don't have lowercase and uppercase.

* toupper_i_dot: When automatically uppercasing, map i to İ, as in Turkish.

* toupper_lower_escape_char: When automatically uppercasing, allow ~ to be used to stop the next letter from being uppercased, for Irish.


== strings.xml ==
This file contains general strings for the interface and some parts of the game. In the XML, the translation is in between the opening and closing <string> tag. For example:

    <string english="Game paused" explanation="pause screen" max="40" comment="">Spel gepauzeerd</string>

The following attributes may be found for each string:

* english: the English text.

* explanation: an explanation about the context, location and possibly the formatting.

* max: length restrictions, described above in "WORDWRAPPING AND LENGTH LIMITS"

* comment: some extra comments you can fill in as a translator. TODO: Will be kept when syncing. It obviously should.


== numbers.xml ==
This file contains all numbers from 0 to 100 written out (Zero, One, etc).

This will be filled in strings like:
- One out of Fifteen
- Two crewmates remaining
- Two remaining

If this can't work for your language ("Twenty out of Twenty" uses two different "Twenty"), you can leave all of these empty, in which case numbers will be used (20 out of 20).

You may want to do it all-lowercase in order to not get English-style title casing. ("Twenty-one out of Twenty-one" may be grammatically incorrect in MANY languages, and "twenty-one out of twenty-one" would be better)


== cutscenes.xml ==
This file contains nearly all the cutscenes that appear in the main game. Each line has a "speaker" attribute, which is not used by the game - it's just for translators to know who says what and to establish context.

The dialogues are automatically text-wrapped, except if automatic wrapping is disabled in meta.xml. In that case, the maximum line length is 36 8x8 characters (288 pixels) or 24 12x12 characters.

TODO: certain specific ones aren't automatically wrapped


== roomnames.xml ==
This file contains nearly all the room names for the main game. The limit is always 40 8x8 characters (320 pixels) or 26 12x12 characters.

It's recommended to translate the room names in-game to see why all rooms are called what they are. To do this, enable room name translation mode in translator > translator options > translate room names.

If you do want to work in the XML for this directly, know that for technical reasons, the tags are self-closing when they're not translated. So this is an untranslated room name:

    <roomname x="4" y="2" english="AAAAAA" explanation=""/>

And this is translated:

    <roomname x="4" y="2" english="AAAAAA" explanation="">Aaaaaa.</roomname>


== roomnames_special.xml ==
This file contains some special cases for roomnames.

One room ("Prize for the Reckless") is intentionally missing spikes in a time trial and no death mode so the player does not have to die there, and the room is called differently in both cases (for time trial "Imagine Spikes There, if You Like", and for no death mode "I Can't Believe You Got This Far").

There are also some roomnames in the game which gradually transform into others or cycle through a few minor variations.

