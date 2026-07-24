/**
 * @file enigmacrack/repl.c
 *
 * Interactive REPL for cracking Enigma machine configurations.
 *
 * Commands (all case-insensitive):
 *   lddict  <file>          ldd    Load dictionary file
 *   ldfreq  <file>          ldf    Load letter-frequency file
 *   ldngram <file>          ldn    Load n-gram file
 *   ldcipher <file>         ldc    Load ciphertext from file
 *   score                   sc     Show top 10 scores from last analysis
 *   set method <ioc|ngram>  sm     Set scoring method
 *   set lang <english|german>      Set IOC score range for language
 *   set minscore <f>               Set minimum IOC score threshold
 *   set maxscore <f>               Set maximum IOC score threshold
 *   set cipher  <text>             Set ciphertext inline
 *   set rotors  <I II III>         Set rotor configuration
 *   set positions <ABC>            Set rotor starting positions
 *   set reflector <A|B|C>          Set reflector
 *   set plugboard <ABCD>           Set plugboard pairs
 *   attribute <name> on|off  attr  Toggle analysis attribute
 *     Names: r0 r1 r2 r3  r0p r1p r2p r3p  reflector  plugboard
 *   analyze                  an    Run analysis over varied attributes
 *   show config              cfg   Show current Enigma + REPL state
 *   show vary                vars  Show which attributes are varied
 *   show scores                    Same as 'score'
 *   clear [scores]                 Clear the score list
 *   help                    h  ?   This help text
 *   exit                    quit q Quit
 */
#include "repl.h"

#include "enigma/crack.h"
#include "enigma/enigma.h"
#include "enigma/io.h"
#include "enigma/ioc.h"
#include "enigma/ngram.h"
#include "enigma/score.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Internal types
 * ───────────────────────────────────────────────────────────────────────── */
typedef float (*ScoreFunc)(const EnigmaCrackParams*, const char*);

/* ─────────────────────────────────────────────────────────────────────────
 * REPL state
 * ───────────────────────────────────────────────────────────────────────── */
#define INPUT_BUF_SIZE  512
#define INITIAL_SCORES  64

static EnigmaCrackParams g_cfg;
static int               g_method    = REPL_METHOD_IOC;
static int               g_vary      = 0;
static char              g_cipher[REPL_CIPHER_MAX];
static int               g_freq_loaded = 0;

/* ─────────────────────────────────────────────────────────────────────────
 * Forward declarations
 * ───────────────────────────────────────────────────────────────────────── */
static void        cmd_lddict(const char*);
static void        cmd_ldfreq(const char*);
static void        cmd_ldngram(const char*);
static void        cmd_ldcipher(const char*);
static void        cmd_score(void);
static void        cmd_set(const char*);
static void        cmd_attribute(const char*);
static void        cmd_analyze(void);
static void        cmd_show(const char*);
static void        cmd_clear(const char*);
static void        cmd_help(void);

static void        set_cipher_str(const char*);
static void        set_cipher_file(const char*);
static void        print_scores_top(int);
static ScoreFunc   pick_score_func(void);
static void        lower_copy(char*, const char*, size_t);
static const char* skip_ws(const char*);
static void        extract_token(const char*, char*, size_t, const char**);

/* ─────────────────────────────────────────────────────────────────────────
 * Entry point
 * ───────────────────────────────────────────────────────────────────────── */
int start_repl(void) {
    /* Initialise Enigma config */
    memset(&g_cfg, 0, sizeof(EnigmaCrackParams));
    enigma_init_default_config(&g_cfg.enigma);
    g_cipher[0] = '\0';

    /* Allocate score list on the heap so enigma_score_append can realloc */
    g_cfg.score_list              = malloc(sizeof(EnigmaScoreList));
    g_cfg.score_list->scores      = malloc(INITIAL_SCORES * sizeof(EnigmaScore));
    g_cfg.score_list->score_count = 0;
    g_cfg.score_list->max_scores  = INITIAL_SCORES;

    /* Defaults */
    g_method             = REPL_METHOD_IOC;
    g_vary               = 0;
    g_freq_loaded        = 0;
    g_cfg.min_score      = ENIGMA_IOC_ENGLISH_MIN;
    g_cfg.max_score      = ENIGMA_IOC_ENGLISH_MAX;

    printf("enigmacrack REPL  (libenigma %s)\n", enigma_version());
    printf("Type 'help' or '?' for available commands.\n\n");

    char input[INPUT_BUF_SIZE];
    int  running = 1;

    while (running) {
        printf("enigmacrack> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break; /* EOF */
        }

        /* Strip trailing whitespace / newline */
        int len = (int)strlen(input);
        while (len > 0 && isspace((unsigned char)input[len - 1]))
            input[--len] = '\0';

        if (len == 0)
            continue;

        /* Extract lowercased first token and original-case remainder */
        char        first[64] = { 0 };
        const char* args      = NULL;
        extract_token(input, first, sizeof(first), &args);

        /* ── Command dispatch ─────────────────────────────────────────── */
        if (!strcmp(first, "exit") || !strcmp(first, "quit") || !strcmp(first, "q")) {
            running = 0;

        } else if (!strcmp(first, "lddict") || !strcmp(first, "ldd")) {
            cmd_lddict(args);

        } else if (!strcmp(first, "ldfreq") || !strcmp(first, "ldf")) {
            cmd_ldfreq(args);

        } else if (!strcmp(first, "ldngram") || !strcmp(first, "ldn")) {
            cmd_ldngram(args);

        } else if (!strcmp(first, "ldcipher") || !strcmp(first, "ldc")) {
            cmd_ldcipher(args);

        } else if (!strcmp(first, "score") || !strcmp(first, "sc")) {
            cmd_score();

        } else if (!strcmp(first, "set")) {
            cmd_set(args);

        /* 'sm <method>' — shorthand for 'set method' */
        } else if (!strcmp(first, "sm")) {
            char tmp[INPUT_BUF_SIZE];
            snprintf(tmp, sizeof(tmp), "method %s", args ? args : "");
            cmd_set(tmp);

        } else if (!strcmp(first, "attribute") || !strcmp(first, "attr")) {
            cmd_attribute(args);

        } else if (!strcmp(first, "analyze") || !strcmp(first, "analyse") ||
                   !strcmp(first, "an")) {
            cmd_analyze();

        } else if (!strcmp(first, "show")) {
            cmd_show(args);

        /* Shorthands for 'show ...' */
        } else if (!strcmp(first, "cfg") || !strcmp(first, "config")) {
            cmd_show("config");

        } else if (!strcmp(first, "vars") || !strcmp(first, "vary")) {
            cmd_show("vary");

        } else if (!strcmp(first, "clear")) {
            cmd_clear(args);

        } else if (!strcmp(first, "help") || !strcmp(first, "h") || !strcmp(first, "?")) {
            cmd_help();

        } else {
            printf("Unknown command: '%s'. Type 'help' for usage.\n", first);
        }
    }

    /* Cleanup */
    if (g_cfg.dictionary)
        enigma_free_dict(&g_cfg);
    if (g_cfg.ngrams)
        free(g_cfg.ngrams);
    free(g_cfg.score_list->scores);
    free(g_cfg.score_list);

    return 0;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Command implementations
 * ───────────────────────────────────────────────────────────────────────── */

static void cmd_lddict(const char* path) {
    if (!path || !*path) {
        printf("Usage: lddict <file>  (ldd)\n");
        return;
    }
    /* Free any existing dictionary first */
    if (g_cfg.dictionary)
        enigma_free_dict(&g_cfg);
    g_cfg.dictionary = NULL;

    if (enigma_load_dict_f(&g_cfg, path)) {
        printf("Error: failed to load dictionary from '%s'.\n", path);
    } else {
        printf("Dictionary loaded from '%s'.\n", path);
    }
}

static void cmd_ldfreq(const char* path) {
    if (!path || !*path) {
        printf("Usage: ldfreq <file>  (ldf)\n");
        printf("File format: one 'LETTER FREQUENCY' pair per line (e.g. 'A 0.08167').\n");
        return;
    }
    FILE* f = fopen(path, "r");
    if (!f) {
        printf("Error: cannot open '%s'.\n", path);
        return;
    }

    char  line[64];
    int   loaded = 0;
    float freqs[26];
    memset(freqs, 0, sizeof(freqs));

    while (fgets(line, sizeof(line), f) && loaded < 26) {
        char  letter;
        float freq;
        if (sscanf(line, " %c %f", &letter, &freq) == 2) {
            int idx = toupper((unsigned char)letter) - 'A';
            if (idx >= 0 && idx < 26) {
                freqs[idx] = freq;
                loaded++;
            }
        }
    }
    fclose(f);

    if (loaded == 0) {
        printf("Error: no valid frequency entries found in '%s'.\n", path);
        return;
    }
    if (loaded < 26) {
        printf("Warning: only %d/26 letter frequencies loaded from '%s'.\n", loaded, path);
    }

    memcpy(g_cfg.frequency_targets, freqs, sizeof(freqs));
    /* Default tolerance: 1 % */
    g_cfg.frequency_offset = 0.01f;
    /* Enable frequency matching in scoring (bit 2 = ENIGMA_FLAG_FREQUENCY) */
    g_cfg.flags |= 2;
    g_freq_loaded = 1;
    printf("Frequency table loaded (%d entries) from '%s'.\n", loaded, path);
}

static void cmd_ldngram(const char* path) {
    if (!path || !*path) {
        printf("Usage: ldngram <file>  (ldn)\n");
        return;
    }
    /* Free previous n-gram data */
    if (g_cfg.ngrams) {
        free(g_cfg.ngrams);
        g_cfg.ngrams = NULL;
    }
    if (enigma_load_ngrams(&g_cfg, path)) {
        printf("Error: failed to load n-grams from '%s'.\n", path);
    } else {
        printf("N-grams (n=%d) loaded from '%s'.\n", g_cfg.n, path);
    }
}

static void cmd_ldcipher(const char* path) {
    if (!path || !*path) {
        printf("Usage: ldcipher <file>  (ldc)\n");
        return;
    }
    set_cipher_file(path);
}

static void cmd_score(void) {
    print_scores_top(REPL_DISPLAY_SCORES);
}

/* ── set <attribute> [value] ────────────────────────────────────────────── */
static void cmd_set(const char* args) {
    if (!args || !*args) {
        printf("Usage: set <attribute> [value]\n");
        printf("Attributes: method, lang, minscore, maxscore, cipher, rotors,\n");
        printf("            positions, reflector, plugboard\n");
        return;
    }

    char        attr[64] = { 0 };
    const char* val      = NULL;
    extract_token(args, attr, sizeof(attr), &val);

    /* ── method ── */
    if (!strcmp(attr, "method") || !strcmp(attr, "meth") || !strcmp(attr, "m")) {
        if (!val || !*val) {
            printf("Usage: set method <ioc|ngram>\n");
            return;
        }
        char mlow[16] = { 0 };
        lower_copy(mlow, val, sizeof(mlow));
        if (!strcmp(mlow, "ioc")) {
            g_method = REPL_METHOD_IOC;
            printf("Method set to IOC.\n");
        } else if (!strcmp(mlow, "ngram")) {
            g_method = REPL_METHOD_NGRAM;
            printf("Method set to ngram.\n");
        } else {
            printf("Unknown method '%s'. Use 'ioc' or 'ngram'.\n", val);
        }

    /* ── lang ── */
    } else if (!strcmp(attr, "lang") || !strcmp(attr, "language")) {
        if (!val || !*val) {
            printf("Usage: set lang <english|german>\n");
            return;
        }
        char llow[16] = { 0 };
        lower_copy(llow, val, sizeof(llow));
        if (!strcmp(llow, "english")) {
            g_cfg.min_score = ENIGMA_IOC_ENGLISH_MIN;
            g_cfg.max_score = ENIGMA_IOC_ENGLISH_MAX;
            printf("Language: English (IOC range %.4f \xe2\x80\x93 %.4f).\n",
                   g_cfg.min_score, g_cfg.max_score);
        } else if (!strcmp(llow, "german")) {
            g_cfg.min_score = ENIGMA_IOC_GERMAN_MIN;
            g_cfg.max_score = ENIGMA_IOC_GERMAN_MAX;
            printf("Language: German  (IOC range %.4f \xe2\x80\x93 %.4f).\n",
                   g_cfg.min_score, g_cfg.max_score);
        } else {
            printf("Unknown language '%s'. Use 'english' or 'german'.\n", val);
        }

    /* ── minscore / maxscore ── */
    } else if (!strcmp(attr, "minscore") || !strcmp(attr, "mins")) {
        if (!val || !*val) { printf("Usage: set minscore <float>\n"); return; }
        g_cfg.min_score = (float)atof(val);
        printf("Min score set to %.6f.\n", g_cfg.min_score);

    } else if (!strcmp(attr, "maxscore") || !strcmp(attr, "maxs")) {
        if (!val || !*val) { printf("Usage: set maxscore <float>\n"); return; }
        g_cfg.max_score = (float)atof(val);
        printf("Max score set to %.6f.\n", g_cfg.max_score);

    /* ── cipher ── */
    } else if (!strcmp(attr, "cipher") || !strcmp(attr, "ciphertext") || !strcmp(attr, "ct")) {
        if (!val || !*val) { printf("Usage: set cipher <text>\n"); return; }
        set_cipher_str(val);

    /* ── rotors ── */
    } else if (!strcmp(attr, "rotors") || !strcmp(attr, "rots") || !strcmp(attr, "w")) {
        if (!val || !*val) {
            printf("Usage: set rotors <I II III>  (e.g. 'I II III', 'IV V VI')\n");
            return;
        }
        /* enigma_load_rotor_config mutates its string via strtok — pass a copy */
        char buf[64] = { 0 };
        strncpy(buf, val, sizeof(buf) - 1);
        if (enigma_load_rotor_config(&g_cfg.enigma, buf)) {
            printf("Error: invalid rotor spec '%s'.\n", val);
            printf("Valid rotor names: I, II, III, IV, V, VI, VII, VIII\n");
        } else {
            printf("Rotors set to: %s\n", val);
        }

    /* ── positions ── */
    } else if (!strcmp(attr, "positions") || !strcmp(attr, "pos") || !strcmp(attr, "p")) {
        if (!val || !*val) {
            printf("Usage: set positions <ABC>  (one letter per rotor, e.g. 'AAA')\n");
            return;
        }
        if (enigma_load_rotor_positions(&g_cfg.enigma, val)) {
            printf("Error: invalid positions '%s'. Must be one alpha char per rotor.\n", val);
        } else {
            printf("Rotor positions set to: %s\n", val);
        }

    /* ── reflector ── */
    } else if (!strcmp(attr, "reflector") || !strcmp(attr, "refl") || !strcmp(attr, "u")) {
        if (!val || !*val) {
            printf("Usage: set reflector <A|B|C>\n");
            return;
        }
        if (enigma_load_reflector_config(&g_cfg.enigma, val)) {
            printf("Error: invalid reflector '%s'. Valid: A, B, C\n", val);
        } else {
            printf("Reflector set to: %s\n", val);
        }

    /* ── plugboard ── */
    } else if (!strcmp(attr, "plugboard") || !strcmp(attr, "plug") || !strcmp(attr, "s")) {
        if (!val || !*val) {
            printf("Usage: set plugboard <ABCD>  (letter pairs, e.g. 'ABCD' for A\xe2\x86\x94""B, C\xe2\x86\x94""D)\n");
            return;
        }
        if (enigma_load_plugboard_config(&g_cfg.enigma, val)) {
            printf("Error: invalid plugboard '%s'. Must be an even-length all-alpha string.\n", val);
        } else {
            printf("Plugboard set to: %s\n", val);
        }

    } else {
        printf("Unknown attribute '%s'.\n", attr);
        printf("Valid: method, lang, minscore, maxscore, cipher, rotors, positions,\n");
        printf("       reflector, plugboard\n");
    }
}

/* ── attribute <name> <on|off> ──────────────────────────────────────────── */
static void cmd_attribute(const char* args) {
    if (!args || !*args) {
        printf("Usage: attribute <name> <on|off>  (attr)\n");
        printf("Names: r0 r1 r2 r3  r0p r1p r2p r3p  reflector  plugboard\n");
        return;
    }

    char        name[32] = { 0 };
    const char* rest     = NULL;
    extract_token(args, name, sizeof(name), &rest);

    char state[8] = { 0 };
    lower_copy(state, (rest && *rest) ? rest : "", sizeof(state));

    if (!*state) {
        printf("Usage: attribute %s <on|off>\n", name);
        return;
    }

    int turn_on;
    if (!strcmp(state, "on") || !strcmp(state, "1") || !strcmp(state, "yes")) {
        turn_on = 1;
    } else if (!strcmp(state, "off") || !strcmp(state, "0") || !strcmp(state, "no")) {
        turn_on = 0;
    } else {
        printf("Invalid state '%s'. Use 'on' or 'off'.\n", state);
        return;
    }

    int flag = 0;
    if      (!strcmp(name, "r0"))                              flag = REPL_VARY_R0;
    else if (!strcmp(name, "r1"))                              flag = REPL_VARY_R1;
    else if (!strcmp(name, "r2"))                              flag = REPL_VARY_R2;
    else if (!strcmp(name, "r3"))                              flag = REPL_VARY_R3;
    else if (!strcmp(name, "r0p"))                             flag = REPL_VARY_R0P;
    else if (!strcmp(name, "r1p"))                             flag = REPL_VARY_R1P;
    else if (!strcmp(name, "r2p"))                             flag = REPL_VARY_R2P;
    else if (!strcmp(name, "r3p"))                             flag = REPL_VARY_R3P;
    else if (!strcmp(name, "reflector") || !strcmp(name, "refl")) flag = REPL_VARY_REFLECTOR;
    else if (!strcmp(name, "plugboard") || !strcmp(name, "plug")) flag = REPL_VARY_PLUGBOARD;
    else {
        printf("Unknown attribute '%s'.\n", name);
        printf("Valid: r0, r1, r2, r3, r0p, r1p, r2p, r3p, reflector, plugboard\n");
        return;
    }

    if (turn_on) {
        g_vary |= flag;
        printf("Attribute '%s' ON  — will be varied in analysis.\n", name);
    } else {
        g_vary &= ~flag;
        printf("Attribute '%s' OFF — will be held fixed.\n", name);
    }
}

/* ── analyze ─────────────────────────────────────────────────────────────── */
static void cmd_analyze(void) {
    /* ── Pre-flight checks ── */
    if (!g_cfg.ciphertext || g_cfg.ciphertext_length == 0) {
        printf("Error: no ciphertext set.\n");
        printf("  Use 'set cipher <text>' or 'ldcipher <file>'.\n");
        return;
    }
    if (g_vary == 0) {
        printf("Error: no attributes set to vary.\n");
        printf("  Use 'attribute <name> on' (e.g. 'attribute r0p on').\n");
        return;
    }

    ScoreFunc scoreFunc = pick_score_func();
    if (!scoreFunc)
        return; /* error already reported */

    /* ── Reset score list ── */
    g_cfg.score_list->score_count = 0;

    int rotor_count = g_cfg.enigma.rotor_count;
    int analyzed    = 0;

    /* ── Rotor identity ── */
    int r_on[4] = {
        !!(g_vary & REPL_VARY_R0),
        !!(g_vary & REPL_VARY_R1),
        !!(g_vary & REPL_VARY_R2),
        !!(g_vary & REPL_VARY_R3),
    };
    /* Use the exhaustive crack_rotors when every active slot is enabled */
    int any_rotor     = r_on[0] | r_on[1] | r_on[2] | r_on[3];
    int all_rotors_on = 1;
    for (int i = 0; i < rotor_count; i++) {
        if (!r_on[i]) { all_rotors_on = 0; break; }
    }

    if (any_rotor) {
        if (all_rotors_on) {
            printf("  Cracking all %d rotor slots (exhaustive)...\n", rotor_count);
            enigma_crack_rotors(&g_cfg, scoreFunc);
        } else {
            for (int i = 0; i < 4; i++) {
                if (r_on[i]) {
                    printf("  Cracking rotor in slot %d...\n", i);
                    enigma_crack_rotor(&g_cfg, i, scoreFunc);
                }
            }
        }
        analyzed = 1;
    }

    /* ── Rotor starting positions ── */
    int rp_on[4] = {
        !!(g_vary & REPL_VARY_R0P),
        !!(g_vary & REPL_VARY_R1P),
        !!(g_vary & REPL_VARY_R2P),
        !!(g_vary & REPL_VARY_R3P),
    };
    int any_pos      = rp_on[0] | rp_on[1] | rp_on[2] | rp_on[3];
    int all_pos_on   = 1;
    for (int i = 0; i < rotor_count; i++) {
        if (!rp_on[i]) { all_pos_on = 0; break; }
    }

    if (any_pos) {
        if (all_pos_on) {
            printf("  Cracking all %d rotor positions (exhaustive)...\n", rotor_count);
            enigma_crack_rotor_positions(&g_cfg, scoreFunc);
        } else {
            for (int i = 0; i < 4; i++) {
                if (rp_on[i]) {
                    printf("  Cracking starting position of slot %d...\n", i);
                    enigma_crack_rotor_position(&g_cfg, i, scoreFunc);
                }
            }
        }
        analyzed = 1;
    }

    /* ── Reflector ── */
    if (g_vary & REPL_VARY_REFLECTOR) {
        printf("  Cracking reflector...\n");
        enigma_crack_reflector(&g_cfg, scoreFunc);
        analyzed = 1;
    }

    /* ── Plugboard ── */
    if (g_vary & REPL_VARY_PLUGBOARD) {
        printf("  Cracking plugboard...\n");
        enigma_crack_plugboard(&g_cfg, scoreFunc);
        analyzed = 1;
    }

    if (!analyzed) {
        printf("Nothing to analyze — all vary flags are off?\n");
        return;
    }

    printf("Analysis complete. %d configuration(s) evaluated.\n\n",
           g_cfg.score_list->score_count);
    print_scores_top(REPL_DISPLAY_SCORES);
}

/* ── show <config|vary|scores> ──────────────────────────────────────────── */
static void cmd_show(const char* args) {
    char what[32] = { 0 };
    lower_copy(what, (args && *args) ? args : "", sizeof(what));

    if (!*what || !strcmp(what, "config") || !strcmp(what, "cfg")) {
        /* Enigma config */
        char buf[128];
        enigma_print_config(&g_cfg.enigma, buf);
        printf("Enigma config  : %s\n", buf);
        printf("Method         : %s\n",
               g_method == REPL_METHOD_IOC ? "IOC" : "ngram");
        if (g_method == REPL_METHOD_IOC) {
            printf("IOC range      : %.6f \xe2\x80\x93 %.6f\n",
                   g_cfg.min_score, g_cfg.max_score);
        } else {
            printf("N-gram n       : %d\n", g_cfg.n);
        }
        printf("Ciphertext     : ");
        if (g_cfg.ciphertext && g_cfg.ciphertext_length > 0) {
            int show = (int)g_cfg.ciphertext_length > 52 ? 52 : (int)g_cfg.ciphertext_length;
            printf("%.*s", show, g_cfg.ciphertext);
            if ((int)g_cfg.ciphertext_length > show) printf("...");
            printf("  (%zu chars)\n", g_cfg.ciphertext_length);
        } else {
            printf("(not set)\n");
        }
        printf("Dictionary     : %s\n", g_cfg.dictionary  ? "loaded" : "not loaded");
        printf("N-grams        : %s\n", g_cfg.ngrams       ? "loaded" : "not loaded");
        printf("Freq table     : %s\n", g_freq_loaded      ? "loaded" : "not loaded");
        printf("Scores cached  : %d\n", g_cfg.score_list->score_count);

    } else if (!strcmp(what, "vary") || !strcmp(what, "vars")) {
        static const struct { int flag; const char* name; const char* desc; } attrs[] = {
            { REPL_VARY_R0,        "r0",        "rotor at slot 0 (right-most)" },
            { REPL_VARY_R1,        "r1",        "rotor at slot 1"              },
            { REPL_VARY_R2,        "r2",        "rotor at slot 2"              },
            { REPL_VARY_R3,        "r3",        "rotor at slot 3 (4-rotor)"    },
            { REPL_VARY_R0P,       "r0p",       "starting position of r0"      },
            { REPL_VARY_R1P,       "r1p",       "starting position of r1"      },
            { REPL_VARY_R2P,       "r2p",       "starting position of r2"      },
            { REPL_VARY_R3P,       "r3p",       "starting position of r3"      },
            { REPL_VARY_REFLECTOR, "reflector", "reflector (UKW)"              },
            { REPL_VARY_PLUGBOARD, "plugboard", "plugboard (Steckerbrett)"     },
        };
        printf("Varied attributes:\n");
        for (int i = 0; i < 10; i++) {
            printf("  %-10s  [%s]  %s\n",
                   attrs[i].name,
                   (g_vary & attrs[i].flag) ? " ON" : "OFF",
                   attrs[i].desc);
        }

    } else if (!strcmp(what, "scores") || !strcmp(what, "score")) {
        cmd_score();

    } else {
        printf("Usage: show <config|vary|scores>\n");
    }
}

static void cmd_clear(const char* args) {
    char what[32] = { 0 };
    lower_copy(what, (args && *args) ? args : "scores", sizeof(what));
    if (!strcmp(what, "scores") || !strcmp(what, "score")) {
        g_cfg.score_list->score_count = 0;
        printf("Score list cleared.\n");
    } else {
        printf("Usage: clear [scores]\n");
    }
}

static void cmd_help(void) {
    printf(
        "enigmacrack REPL \xe2\x80\x94 interactive Enigma cryptanalysis\n\n"
        "Data loading:\n"
        "  lddict  <file>    ldd   Dictionary file (one uppercase word per line)\n"
        "  ldfreq  <file>    ldf   Letter-frequency file ('A 0.08167' per line)\n"
        "  ldngram <file>    ldn   N-gram frequency file\n"
        "  ldcipher <file>   ldc   Ciphertext file (reads first line)\n\n"
        "Configuration  (set / s):\n"
        "  set method  <ioc|ngram>        sm <m>    Scoring method\n"
        "  set lang    <english|german>             IOC score range preset\n"
        "  set minscore <f>                         Minimum IOC score\n"
        "  set maxscore <f>                         Maximum IOC score\n"
        "  set cipher  <text>                       Ciphertext (inline)\n"
        "  set rotors  <I II III>                   Rotor configuration\n"
        "  set positions <ABC>                      Rotor starting positions\n"
        "  set reflector <A|B|C>                    Reflector (UKW)\n"
        "  set plugboard <ABCD>                     Plugboard pairs\n\n"
        "Vary attributes  (attribute / attr):\n"
        "  attribute <name> on|off\n"
        "  Names: r0  r1  r2  r3            rotor in slot 0-3\n"
        "         r0p r1p r2p r3p           starting position of slot 0-3\n"
        "         reflector  plugboard\n"
        "  When ALL slots for the current rotor count are ON, an exhaustive\n"
        "  combined search is used; otherwise each slot is cracked independently.\n\n"
        "Analysis:\n"
        "  analyze  an   Crack all varied attributes; show top 10 results\n\n"
        "Display:\n"
        "  score    sc              Top 10 scores from last analysis\n"
        "  show config    cfg       Enigma + REPL state\n"
        "  show vary      vars      Which attributes are varied\n"
        "  show scores              Same as 'score'\n"
        "  clear [scores]           Discard the current score list\n\n"
        "Other:\n"
        "  help  h  ?   This help\n"
        "  exit  quit q Quit\n"
    );
}

/* ─────────────────────────────────────────────────────────────────────────
 * Internal helpers
 * ───────────────────────────────────────────────────────────────────────── */

/**
 * @brief Store ciphertext from a string, uppercasing alpha and replacing
 *        non-alpha, non-space characters with 'X'.  Spaces are skipped.
 */
static void set_cipher_str(const char* s) {
    int n = 0;
    for (int i = 0; s[i] && n < REPL_CIPHER_MAX - 1; i++) {
        unsigned char c = (unsigned char)s[i];
        if (isalpha(c)) {
            g_cipher[n++] = (char)toupper(c);
        } else if (!isspace(c)) {
            g_cipher[n++] = 'X';
        }
        /* spaces are silently skipped */
    }
    g_cipher[n]                = '\0';
    g_cfg.ciphertext           = g_cipher;
    g_cfg.ciphertext_length    = (size_t)n;
    printf("Ciphertext set (%d characters).\n", n);
}

/**
 * @brief Load ciphertext from the first line of a file.
 */
static void set_cipher_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        printf("Error: cannot open '%s'.\n", path);
        return;
    }
    char buf[REPL_CIPHER_MAX];
    if (!fgets(buf, sizeof(buf), f)) {
        fclose(f);
        printf("Error: '%s' is empty.\n", path);
        return;
    }
    fclose(f);
    buf[strcspn(buf, "\r\n")] = '\0';
    set_cipher_str(buf);
}

/**
 * @brief Sort the score list and print the top n entries.
 */
static void print_scores_top(int n) {
    if (!g_cfg.score_list || g_cfg.score_list->score_count == 0) {
        printf("No scores yet. Run 'analyze' first.\n");
        return;
    }
    enigma_score_list_sort(g_cfg.score_list);
    int display = g_cfg.score_list->score_count < n
                    ? g_cfg.score_list->score_count : n;

    printf("Top %d / %d:\n", display, g_cfg.score_list->score_count);
    printf("  %-3s  %-12s  %-3s  %s\n", "#", "Score", "Fl.", "Config");
    printf("  ---  ------------  ---  ------\n");

    char buf[128];
    for (int i = 0; i < display; i++) {
        EnigmaScore* s = &g_cfg.score_list->scores[i];
        enigma_print_config(&s->enigma, buf);
        char flags[4] = "---";
        if (s->flags & ENIGMA_FLAG_DICTIONARY_MATCH) flags[0] = 'D';
        if (s->flags & ENIGMA_FLAG_FREQUENCY)        flags[1] = 'F';
        if (s->flags & ENIGMA_FLAG_KNOWN_PLAINTEXT)  flags[2] = 'P';
        printf("  %-3d  %-12.6f  %s  %s\n", i + 1, s->score, flags, buf);
    }
}

/**
 * @brief Return the scoring function for the current method, or NULL on error.
 */
static ScoreFunc pick_score_func(void) {
    if (g_method == REPL_METHOD_IOC) {
        return enigma_ioc_score;
    }
    if (g_method == REPL_METHOD_NGRAM) {
        if (!g_cfg.ngrams) {
            printf("Error: no n-grams loaded. Use 'ldngram <file>'.\n");
            return NULL;
        }
        switch (g_cfg.n) {
        case 2: return enigma_bigram_score;
        case 3: return enigma_trigram_score;
        case 4: return enigma_quadgram_score;
        default:
            printf("Error: n-gram length %d is not supported (must be 2–4).\n", g_cfg.n);
            return NULL;
        }
    }
    printf("Error: unknown method %d.\n", g_method);
    return NULL;
}

/**
 * @brief Copy src into dest (up to n-1 bytes), converting to lower-case.
 */
static void lower_copy(char* dest, const char* src, size_t n) {
    size_t i = 0;
    while (i < n - 1 && src[i]) {
        dest[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

/**
 * @brief Return a pointer past any leading whitespace in s.
 */
static const char* skip_ws(const char* s) {
    while (*s && isspace((unsigned char)*s))
        s++;
    return s;
}

/**
 * @brief Extract the first whitespace-delimited token from s.
 *
 * The token is written into tok (lowercased, NUL-terminated up to tok_sz).
 * If rest is non-NULL, *rest is set to the next non-whitespace character
 * after the token (preserving original case).
 */
static void extract_token(const char* s, char* tok, size_t tok_sz, const char** rest) {
    s = skip_ws(s);
    size_t i = 0;
    while (i < tok_sz - 1 && *s && !isspace((unsigned char)*s)) {
        tok[i++] = (char)tolower((unsigned char)*s++);
    }
    tok[i] = '\0';
    /* Skip any characters overflowed due to tok_sz */
    while (*s && !isspace((unsigned char)*s))
        s++;
    if (rest)
        *rest = skip_ws(s);
}
