#include "enigma/crack.h"
#include "enigma/enigma.h"
#include "enigma/io.h"
#include "enigma/ioc.h"
#include "enigma/ngram.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define USAGE                                                                                      \
    "Usage: %s method target [options] ciphertext\n\
    Methods:\n\
      ioc              Use Index of Coincidence for cryptanalysis (target, -m/-M OR -l required)\n\
      ngram            Use n-gram analysis for cryptanalysis (target, -n, -m/-M required)\n\
    Targets:\n\
      rotor[1-3]       Crack a rotor (Walzen) configuration\n\
      position[1-3]    Crack an initial rotor position\n\
      reflector        Crack the reflector (Umkehrwalze) configuration\n\
      plugboard        Crack a plugboard (Steckerbrett) setting\n\n\
    Options:\n\
      Enigma Settings:\n\
        -w rotors      Set the rotor (Walzen) configuration (e.g. 'I II III')\n\
        -p positions   Set the initial rotor positions (e.g. 'ABC')\n\
        -u reflector   Set the reflector (Umkehrwalze) (e.g. 'B')\n\
        -s plugboard   Set the plugboard (Steckerbrett) configuration (e.g. 'ABCDEF')\n\
      Cryptanalysis Settings:\n\
        -c plaintext   Set the known plaintext\n\
        -d file        Set the dictionary file to use\n\
        -l language    Language ('english' or 'german', for IOC method)\n\
        -m float       Minimum score threshold\n\
        -M float       Maximum score threshold\n\
        -n file        n-gram bank to load\n\
        -x             Assume X-separated words in plaintext\n\n\
    A file can be provided as the last argument to read the ciphertext from a file.\n\
    If no file is provided, the ciphertext will be read from standard input.\n\n\
    Note that dictionaries must contain one word per line, be sorted alphabetically, and\n\
    be all uppercase\n\n\
    Available languages: english, german\n\
    Available rotors: I, II, III, IV, V, VI, VII, VIII\n\
    Available reflectors: A, B, C\n"

static void clean_exit(const char*, const char*, EnigmaCrackParams*, int);
static void free_dictionary(char**, int);
static void load_dictionary(EnigmaCrackParams*, const char*);
static void load_frequencies(EnigmaCrackParams*, const char*);
static int  load_language(EnigmaCrackParams*, const char*);
static void load_target(EnigmaCrackParams*, const char*);

#define METHOD_IOC   1
#define METHOD_NGRAM 2

#define TARGET_ROTOR       1
#define TARGET_ROTOR_S     "rotor"
#define TARGET_ROTORS      2
#define TARGET_ROTORS_S    "rotors"
#define TARGET_POSITION    3
#define TARGET_POSITION_S  "position"
#define TARGET_POSITIONS   4
#define TARGET_POSITIONS_S "positions"
#define TARGET_REFLECTOR   5
#define TARGET_REFLECTOR_S "reflector"
#define TARGET_PLUGBOARD   6
#define TARGET_PLUGBOARD_S "plugboard"

#define IS_TARGET(s) (!strncmp(argv[0], s, strlen(s)))

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, USAGE, argv[0]);
        return 1;
    }

    EnigmaCrackParams* cfg = calloc(1, sizeof(EnigmaCrackParams));
    enigma_init_default_config(&cfg->enigma);
    cfg->ciphertext          = argv[argc - 1];
    cfg->ciphertext_length   = strlen(cfg->ciphertext);
    cfg->enigma.plugboard[0] = '\0';
    int method               = 0;
    int target               = 0;
    int param                = 0;

    if (!strcmp(argv[1], "ioc")) {
        method = METHOD_IOC;
    } else if (!strcmp(argv[1], "ngram")) {
        method = METHOD_NGRAM;
    } else {
        fprintf(stderr, "Unknown method: %s\n", argv[1]);
        fprintf(stderr, USAGE, argv[0]);
        free(cfg);
        return 1;
    }

    if (IS_TARGET(TARGET_ROTORS_S)) {
        target = TARGET_ROTORS;
    } else if (IS_TARGET(TARGET_ROTOR_S)) {
        target = TARGET_ROTOR;
        if (strlen(argv[2]) < strlen(TARGET_ROTOR_S) + 1) {
            clean_exit("\"rotor\" target must be followed by a rotor number\n", argv[0], cfg, 1);
        }
        param = argv[2][strlen(TARGET_ROTOR_S)] - '0';

        if (param < 1 || param > 3) {
            clean_exit("\"rotor\" target requires a rotor number between 1 and 3\n",
                       argv[0],
                       cfg,
                       1);
        }
    } else if (IS_TARGET(TARGET_POSITIONS_S)) {
        target = TARGET_POSITIONS;
    } else if (IS_TARGET(TARGET_POSITION_S)) {
        target = TARGET_POSITION;
        if (strlen(argv[2]) < strlen(TARGET_POSITION_S) + 1) {
            clean_exit("\"position\" target must be followed by a rotor number\n", argv[0], cfg, 1);
        }
        param = argv[2][strlen(TARGET_POSITION_S)] - '0';

        if (param < 1 || param > 3) {
            clean_exit("\"position\" target requires a rotor number between 1 and 3\n",
                       argv[0],
                       cfg,
                       1);
        }
    } else if (IS_TARGET(TARGET_REFLECTOR_S)) {
        target = TARGET_REFLECTOR;
    } else if (IS_TARGET(TARGET_PLUGBOARD_S)) {
        target = TARGET_PLUGBOARD;
    } else {
        fprintf(stderr, "Unknown target: %s\n", argv[2]);
        fprintf(stderr, USAGE, argv[0]);
        free(cfg);
        return 1;
    }

    optind += 2;
    int opt;
    while ((opt = getopt(argc, argv, "w:p:u:s:c:d:l:m:M:n:f:x")) != -1) {
        switch (opt) {
        case 'w':
            enigma_load_rotor_config(&cfg->enigma, optarg);
            break;
        case 'p':
            enigma_load_rotor_positions(&cfg->enigma, optarg);
            break;
        case 'u':
            enigma_load_reflector_config(&cfg->enigma, optarg);
            break;
        case 's':
            strcpy(cfg->enigma.plugboard, optarg);
            break;
        case 'c':
            cfg->flags |= ENIGMA_FLAG_KNOWN_PLAINTEXT;
            cfg->known_plaintext = optarg;
            break;
        case 'd':
            load_dictionary(cfg, optarg);
            break;
        case 'l':
            if (load_language(cfg, optarg)) {
                fprintf(stderr, "Unknown language: %s\n", optarg);
                clean_exit(NULL, argv[0], cfg, 1);
            }
            break;
        case 'm':
            cfg->min_score = atof(optarg);
            break;
        case 'M':
            cfg->max_score = atof(optarg);
            break;
        case 'n':
            enigma_load_ngrams(cfg, optarg);
            break;
        case 'f':
            load_frequencies(cfg, optarg);
            break;
        case 'x':
            cfg->flags |= ENIGMA_FLAG_X_SEPARATED;
            break;
        default:
            clean_exit("Error: Unknown option", argv[0], cfg, 1);
        }
    }

    cfg->score_list              = malloc(sizeof(EnigmaScoreList));
    cfg->score_list->scores      = malloc(100 * sizeof(EnigmaScore));
    cfg->score_list->score_count = 0;
    cfg->score_list->max_scores  = 100;

    if (method == METHOD_IOC) {
        if (!cfg->min_score || !cfg->max_score) {
            clean_exit("IOC method requires -m and -M options (or -l to set language)\n",
                       argv[0],
                       cfg,
                       1);
        }

        switch (target) {
        case TARGET_ROTOR:
            enigma_crack_rotor(cfg, param - 1, enigma_ioc_score);
            break;
        case TARGET_ROTORS:
            enigma_crack_rotors(cfg, enigma_ioc_score);
            break;
        case TARGET_POSITION:
            enigma_crack_rotor_position(cfg, param - 1, enigma_ioc_score);
            break;
        case TARGET_POSITIONS:
            enigma_crack_rotor_positions(cfg, enigma_ioc_score);
            break;
        case TARGET_REFLECTOR:
            enigma_crack_reflector(cfg, enigma_ioc_score);
            break;
        case TARGET_PLUGBOARD:
            enigma_crack_plugboard(cfg, enigma_ioc_score);
            break;
        }
    } else if (method == METHOD_NGRAM) {
        if (!cfg->ngrams) {
            clean_exit("N-gram method requires -n option\n", argv[0], cfg, 1);
        }

        float (*scoreFunc)(const EnigmaCrackParams*, const char*) = NULL;

        switch (cfg->n) {
        case 2:
            scoreFunc = enigma_bigram_score;
            break;
        case 3:
            scoreFunc = enigma_trigram_score;
            break;
        case 4:
            scoreFunc = enigma_quadgram_score;
            break;
        default:
            clean_exit("Invalid n-gram length. Must be between 2 and 4.\n", argv[0], cfg, 1);
        }

        switch (target) {
        case TARGET_ROTOR:
            if (param < 1 || param > 3) {
                clean_exit("Rotor target requires rotor number (1-3)\n", argv[0], cfg, 1);
            }
            enigma_crack_rotor(cfg, param - 1, scoreFunc);
            break;
        case TARGET_ROTORS:
            enigma_crack_rotors(cfg, scoreFunc);
            break;
        case TARGET_POSITION:
            if (param < 1 || param > 3) {
                clean_exit("Position target requires rotor number (1-3)\n", argv[0], cfg, 1);
            }
            enigma_crack_rotor_position(cfg, param - 1, scoreFunc);
            break;
        case TARGET_POSITIONS:
            enigma_crack_rotor_positions(cfg, scoreFunc);
            break;
        case TARGET_REFLECTOR:
            enigma_crack_reflector(cfg, scoreFunc);
            break;
        case TARGET_PLUGBOARD:
            enigma_crack_plugboard(cfg, scoreFunc);
            break;
        }
    }

    enigma_score_print(cfg->score_list);

    free(cfg);
    return 0;
}

static void clean_exit(const char* msg, const char* argv0, EnigmaCrackParams* cfg, int code) {
    if (msg) {
        fprintf(stderr, "%s", msg);
    }
    fprintf(stderr, USAGE, argv0);
    if (cfg->dictionary) {
        free_dictionary((char**) cfg->dictionary, cfg->dictionary_length);
    }
    free(cfg);
    exit(code);
}

static void free_dictionary(char** dictionary, int size) {
    for (int i = 0; i < size; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
}

static void load_dictionary(EnigmaCrackParams* cfg, const char* path) {
    size_t alloced         = 10000;
    cfg->dictionary        = malloc(alloced * sizeof(char*));
    cfg->dictionary_length = 0;
    FILE* f                = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open dictionary file: %s\n", path);
        return;
    }

    char line[BUFSIZ];
    while ((fgets(line, sizeof(line), f)) != NULL) {
        if (cfg->dictionary_length >= alloced) {
            alloced *= 2;
            cfg->dictionary = realloc(cfg->dictionary, alloced * sizeof(char*));
        }
        line[strcspn(line, "\n")]               = 0;
        cfg->dictionary[cfg->dictionary_length] = strdup(line);
        cfg->dictionary_length++;
    }
    fclose(f);
}

/**
 * @brief Load language settings.
 *
 * This function sets the minimum and maximum score thresholds based on the specified language.
 * Supports "english" and "german".
 *
 * @param config Pointer to the enigma_crack_config_t structure to update.
 * @param language The language to load settings for (e.g., "english").
 *
 * @return 0 on success, 1 if the language is not recognized.
 */
static int load_language(EnigmaCrackParams* config, const char* language) {
    if (!strcmp(language, "english")) {
        config->min_score = ENIGMA_IOC_ENGLISH_MIN;
        config->max_score = ENIGMA_IOC_ENGLISH_MAX;
        return 0;
    } else if (!strcmp(language, "german")) {
        config->min_score = ENIGMA_IOC_GERMAN_MIN;
        config->max_score = ENIGMA_IOC_GERMAN_MAX;
        return 0;
    }

    return 1;
}

static void load_frequencies(EnigmaCrackParams* config, const char* path) {
    // TODO Implement
    fprintf(stderr, "Frequency analysis not yet implemented.\n");
}
