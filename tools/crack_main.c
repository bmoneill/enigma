#include "enigma/brute.h"
#include "enigma/common.h"
#include "enigma/crack.h"
#include "enigma/enigma.h"
#include "enigma/io.h"
#include "enigma/ioc.h"
#include "enigma/ngram.h"
#include "enigma/threads.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void                   free_dictionary(char**,                 int);
static void                   load_dictionary(enigma_crack_config_t*, const char*);
static int                    load_language  (enigma_crack_config_t*, const char*);
static void                   load_target    (enigma_crack_config_t*, const char*);
static int                    print_usage    (const char*);
static enigma_crack_config_t* parse_arguments(int,                    char**,     enigma_ngram_list_t*);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    enigma_ngram_list_t* ngramList = malloc(sizeof(enigma_ngram_list_t));
    enigma_crack_config_t* config = parse_arguments(argc, argv, ngramList);

    if (!config) {
        free(ngramList);
        return 1;
    }

    config->letterFreqOffset = 0.01f;
    config->letterFreqTargets = malloc(26 * sizeof(float));

    // English letter frequency targets (from https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html)
    // TODO make this an argument
    // X is probably unreliable here due to possibly being used as a space substitute
    float englishFreq[26] = {
        0.0812f, 0.0149f, 0.0271f, 0.0432f, 0.1202f, 0.023f, 0.0203f, 0.0592f, 0.0731f, 0.0010f,
        0.0069f, 0.0398f, 0.0261f, 0.0695f, 0.0768f, 0.0182f, 0.0011f, 0.0602f, 0.0628f, 0.0910f,
        0.0288f, 0.0111f, 0.0209f, 0.0017f, 0.0211f, 0.0007f
    };

    memcpy(config->letterFreqTargets, englishFreq, 26 * sizeof(float));

    switch (config->method) {
    case ENIGMA_METHOD_BRUTE: enigma_crack_brute(config); break;
    case ENIGMA_METHOD_NGRAM:
        switch (config->target) {
        case ENIGMA_TARGET_ROTORS: enigma_crack_rotors_ngram(config, ngramList); break;
        case ENIGMA_TARGET_POSITIONS: enigma_crack_rotor_positions_ngram(config, ngramList); break;
        case ENIGMA_TARGET_REFLECTOR: enigma_crack_reflector_ngram(config, ngramList); break;
        case ENIGMA_TARGET_PLUGBOARD: enigma_crack_plugboard_ngram(config, ngramList); break;
        }
        enigma_score_sort(enigma_scores);
        enigma_score_print(enigma_scores);
        break;
    case ENIGMA_METHOD_IOC:
        switch (config->target) {
        case ENIGMA_TARGET_ROTORS: enigma_crack_rotors_ioc(config); break;
        case ENIGMA_TARGET_POSITIONS: enigma_crack_rotor_positions_ioc(config); break;
        case ENIGMA_TARGET_REFLECTOR: enigma_crack_reflector_ioc(config); break;
        case ENIGMA_TARGET_PLUGBOARD: enigma_crack_plugboard_ioc(config); break;
        }
        enigma_score_sort(enigma_scores);
        enigma_score_print(enigma_scores);
        break;
    }

    if (config->dictionary) {
        free_dictionary((char**)config->dictionary, config->dictSize);
    }
    if (enigma_scores) {
        free(enigma_scores->scores);
        free(enigma_scores);
    }
    free(ngramList);
    free(config->letterFreqTargets);
    free(config);
    return 0;
}

static void free_dictionary(char** dictionary, int size) {
    for (int i = 0; i < size; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
}

static void load_dictionary(enigma_crack_config_t* cfg, const char* path) {
    int alloced = 10000;
    cfg->dictionary = malloc(alloced * sizeof(char*));
    cfg->dictSize = 0;
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open dictionary file: %s\n", path);
        return;
    }

    char line[BUFSIZ];
    while ((fgets(line, sizeof(line), f)) != NULL) {
        if (cfg->dictSize >= alloced) {
            alloced *= 2;
            cfg->dictionary = realloc(cfg->dictionary, alloced * sizeof(char*));
        }
        line[strcspn(line, "\n")] = 0;
        cfg->dictionary[cfg->dictSize] = strdup(line);
        cfg->dictSize++;
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
static int load_language(enigma_crack_config_t *config, const char *language) {
    if (!strcmp(language, "english")) {
        config->minScore = enigma_ioc_english_min;
        config->maxScore = enigma_ioc_english_max;
        return 0;
    } else if (!strcmp(language, "german")) {
        config->minScore = enigma_ioc_german_min;
        config->maxScore = enigma_ioc_german_max;
        return 0;
    }

    return 1;
}

static void load_target(enigma_crack_config_t* config, const char* target) {
    const char* targets[] = {"rotor", "position", "reflector", "plugboard"};
    if (config->method == ENIGMA_METHOD_IOC || config->method == ENIGMA_METHOD_NGRAM) {
        for (int i = 0; i < 4; i++) {
            if (!strcmp(target, targets[i])) {
                config->target = i;
                config->target_param = target[strlen(target) - 1] - '0';
                return;
            }
        }
    }

}

/**
 * @brief Print usage information.
 *
 * This function prints the command line options and their descriptions.
 *
 * @param argv0 The name of the program, typically `argv[0]`.
 *
 * @return 1
 */
static int print_usage(const char* argv0) {
    fprintf(stderr, "Usage: %s method [target] [options] [ciphertext]\n", argv0);
    fprintf(stderr, "Methods:\n");
    fprintf(stderr, "  bombe            Use a pseudo-Bombe for cryptanalysis (-c, -C required)\n");
    fprintf(stderr, "  brute            Use Brute Force for cryptanalysis\n");
    fprintf(stderr, "  ioc              Use Index of Coincidence for cryptanalysis (target, -m/-M OR -l required)\n");
    fprintf(stderr, "  ngram            Use n-gram analysis for cryptanalysis (target, -n, -m/-M required)\n");
    fprintf(stderr, "Targets:\n");
    fprintf(stderr, "  rotors           Crack the rotor (Walzen) configuration\n");
    fprintf(stderr, "  positions        Crack the initial rotor positions\n");
    fprintf(stderr, "  reflector        Crack the reflector (Umkehrwalze) configuration\n");
    fprintf(stderr, "  plugboard        Crack the plugboard (Steckerbrett) configuration\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  Enigma Settings:\n");
    fprintf(stderr, "    -w rotors      Set the rotor (Walzen) configuration (e.g. 'I II III')\n");
    fprintf(stderr, "    -p positions   Set the initial rotor positions (e.g. 'ABC')\n");
    fprintf(stderr, "    -u reflector   Set the reflector (Umkehrwalze) (e.g. 'B')\n");
    fprintf(stderr, "    -s plugboard   Set the plugboard (Steckerbrett) configuration (e.g. 'ABCDEF')\n");
    fprintf(stderr, "  Cryptanalysis Settings:\n");
    fprintf(stderr, "    -c plaintext   Set the known plaintext\n");
    fprintf(stderr, "    -C position    Set the position of known plaintext\n");
    fprintf(stderr, "    -d file        Set the dictionary file to use\n");
    fprintf(stderr, "    -l language    Language ('english' or 'german', for IOC method)\n");
    fprintf(stderr, "    -m float       Minimum score threshold (for n-gram, IOC, and brute methods)\n");
    fprintf(stderr, "    -M float       Maximum score threshold (for n-gram, IOC, and brute methods)\n");
    fprintf(stderr, "    -T float       Target score (for n-gram, IoC, and brute methods)\n\n");
    fprintf(stderr, "    -n file        n-gram bank to load\n\n");
    fprintf(stderr, "    -S count       Set the maximum number of plugboard settings (default: 8)\n");
    fprintf(stderr, "    -t threadCount Number of threads to use (default: 8)\n\n");
    fprintf(stderr, "Other Options:\n");
    fprintf(stderr, "    -h             Show this help message\n");
    fprintf(stderr, "A file can be provided as the last argument to read the ciphertext from a file.\n");
    fprintf(stderr, "If no file is provided, the ciphertext will be read from standard input.\n\n");
    fprintf(stderr, "Available languages: english, german\n");
    fprintf(stderr, "Available rotors: I, II, III, IV, V, VI, VII, VIII\n");
    fprintf(stderr, "Available reflectors: A, B, C\n");
    return 1;
}

static enigma_crack_config_t *parse_arguments(int argc, char* argv[], enigma_ngram_list_t* ngramList) {
    int opt;
    int result = 0;
    enigma_crack_config_t *config;

    config = calloc(1, sizeof(enigma_crack_config_t));
    enigma_init_default_config(&config->enigma);
    config->maxThreads = 64;

    if (!strcmp(argv[1], "bombe")) {
        config->method = ENIGMA_METHOD_BOMBE;
    } else if (!strcmp(argv[1], "brute")) {
        config->method = ENIGMA_METHOD_BRUTE;
    } else if (!strcmp(argv[1], "ioc")) {
        config->method = ENIGMA_METHOD_IOC;
    } else if (!strcmp(argv[1], "ngram")) {
        config->method = ENIGMA_METHOD_NGRAM;
    } else {
        free(config);
        print_usage(argv[0]);
        return NULL;
    }

    optind++;
    if (config->method == ENIGMA_METHOD_IOC || config->method == ENIGMA_METHOD_NGRAM) {
        if (optind >= argc) {
            free(config);
            print_usage(argv[0]);
            return NULL;
        }
        load_target(config, argv[optind]);
        if (config->target < 0) {
            free(config);
            print_usage(argv[0]);
            return NULL;
        }
        optind++;
    }

    while ((opt = getopt(argc, argv, "c:C:d:l:m:M:n:p:s:S:t:T:u:w:")) != -1) {
        switch (opt) {
        case 'c': config->plaintext = optarg; break;
        case 'C': config->plaintextPos = atoi(optarg); break;
        case 'd': load_dictionary(config, optarg); break;
        case 'l': result = load_language(config, optarg); break;
        case 'm': config->minScore = atof(optarg); break;
        case 'M': config->maxScore = atof(optarg); break;
        case 'n': ngramList = enigma_load_ngrams(optarg); break;
        case 's': config->enigma.plugboard = optarg; break;
        case 'S': config->maxPlugboardSettings = atoi(optarg); break;
        case 't': config->maxThreads = atoi(optarg); break;
        case 'T': config->targetScore = atof(optarg); break;
        case 'p':
            result = enigma_load_rotor_positions(&config->enigma, optarg) == 0;
            config->flags |= ENIGMA_PREDEFINED_ROTOR_POSITIONS;
            break;
        case 'u':
            result = enigma_load_reflector_config(&config->enigma, optarg);
            config->flags |= ENIGMA_PREDEFINED_REFLECTOR;
            break;
        case 'w':
            result = enigma_load_rotor_config(&config->enigma, optarg) == 0;
            config->flags |= ENIGMA_PREDEFINED_ROTOR_SETTINGS;
            break;
        default:  result = print_usage(argv[0]);
        }
    }

    if (result || ((config->method == ENIGMA_METHOD_NGRAM || config->method == ENIGMA_METHOD_IOC) &&
         (config->target < 0 || (!config->minScore && !config->maxScore && !config->ngramCount))) ||
        (config->method == ENIGMA_METHOD_NGRAM && !ngramList)) {
        free(config);
        print_usage(argv[0]);
        return NULL;
    }

    if (optind < argc) {
        config->ciphertext = argv[optind];
        config->ciphertextLen = strlen(config->ciphertext);
    }

    return config;
}

