#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma/common.h"
#include "enigma/enigma.h"
#include "enigma/ioc.h"

enum {
    MODE_BRUTE = 0,
    MODE_IOC,
    MODE_BIGRAM,
    MODE_TRIGRAM,
    MODE_QUADGRAM
};

enum {
    TARGET_ROTORS = 0,
    TARGET_POSITIONS,
    TARGET_REFLECTOR,
    TARGET_PLUGBOARD
};

enum {
    LANG_ENGLISH = 0,
    LANG_GERMAN
};

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {
    int opt;
    enigma_t enigma;
    int mode = -1;
    int target = -1;
    int maxPlugboardSettings = 10; // default
    char* plaintext = NULL;
    int plaintextPos = -1;
    int lang = LANG_ENGLISH; // default
    int threadCount = 8; // default


    while ((opt = getopt(argc, argv, "w:p:u:s:S:c:C:l:t:")) != -1) {
        switch (opt) {
        case 'w': enigma_load_rotor_config(&enigma, optarg); break;
        case 'p': enigma_load_rotor_positions(&enigma, optarg); break;
        case 'u': enigma_load_reflector_config(&enigma, optarg); break;
        case 's': enigma.plugboard = optarg; break;
        case 'S': maxPlugboardSettings = atoi(optarg); break;
        case 'c': plaintext = optarg; break;
        case 'C': plaintextPos = atoi(optarg); break;
        case 't': threadCount = atoi(optarg); break;
        case 'l':
            if (!strcmp(optarg, "english")) {
                lang = LANG_ENGLISH;
            } else if (!strcmp(optarg, "german")) {
                lang = LANG_GERMAN;
            } else {
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    if (optind + 2 >= argc) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!strcmp(argv[optind], "brute")) {
        mode = MODE_BRUTE;
    } else if (!strcmp(argv[optind], "ioc")) {
        mode = MODE_IOC;
    } else if (!strcmp(argv[optind], "bigram")) {
        mode = MODE_BIGRAM;
    } else if (!strcmp(argv[optind], "trigram")) {
        mode = MODE_TRIGRAM;
    } else if (!strcmp(argv[optind], "quadgram")) {
        mode = MODE_QUADGRAM;
    } else {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!strcmp(argv[optind + 1], "rotors")) {
        target = TARGET_ROTORS;
    } else if (!strcmp(argv[optind + 1], "positions")) {
        target = TARGET_POSITIONS;
    } else if (!strcmp(argv[optind + 1], "reflector")) {
        target = TARGET_REFLECTOR;
    } else if (!strcmp(argv[optind + 1], "plugboard")) {
        target = TARGET_PLUGBOARD;
    } else {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Print usage information.
 *
 * This function prints the command line options and their descriptions.
 *
 * @param argv0 The name of the program, typically `argv[0]`.
 *
 */
static void print_usage(const char* argv0) {
    fprintf(stderr, "Usage: %s [options] method target [file]\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -w rotors      Set the rotor (Walzen) configuration (e.g. 'I II III')\n");
    fprintf(stderr, "  -p positions   Set the initial rotor positions (e.g. 'ABC')\n");
    fprintf(stderr, "  -u reflector   Set the reflector (Umkehrwalze) (e.g. 'B')\n");
    fprintf(stderr, "  -s plugboard   Set the plugboard (Steckerbrett) configuration (e.g. 'ABCDEF')\n");
    fprintf(stderr, "  -S count       Set the maximum number of plugboard settings (e.g. '10')\n");
    fprintf(stderr, "  -c plaintext   Known plaintext\n");
    fprintf(stderr, "  -C position    Set the position of known plaintext\n");
    fprintf(stderr, "  -l language    Language ('english' or 'german')\n");
    fprintf(stderr, "  -t threadCount Number of threads to use\n\n");
    fprintf(stderr, "Methods:\n");
    fprintf(stderr, "  brute          Use Brute Force for cryptanalysis\n");
    fprintf(stderr, "  ioc            Use Index of Coincidence for cryptanalysis\n");
    fprintf(stderr, "  bigram         Use Bigram analysis for cryptanalysis\n");
    fprintf(stderr, "  trigram        Use Trigram analysis for cryptanalysis\n");
    fprintf(stderr, "  quadgram       Use Quadgram analysis for cryptanalysis\n\n");
    fprintf(stderr, "Targets:\n");
    fprintf(stderr, "  rotors        Crack the rotor (Walzen) configuration\n");
    fprintf(stderr, "  positions     Crack the initial rotor positions\n");
    fprintf(stderr, "  reflector     Crack the reflector (Umkehrwalze) configuration\n");
    fprintf(stderr, "  plugboard     Crack the plugboard (Steckerbrett) configuration\n\n");
    fprintf(stderr, "Available rotors: I, II, III, IV, V, VI, VII, VIII\n");
    fprintf(stderr, "Available reflectors: A, B, C\n");
    exit(EXIT_FAILURE);
}
