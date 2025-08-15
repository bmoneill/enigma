#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "enigma.h"
#include "ioc.h"

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {
    int opt;
    enigma_t enigma;
//    char *rotorpos = NULL;

    while ((opt = getopt(argc, argv, "p:s:u:w:t:c:C:")) != -1) {
        switch (opt) {
        case 's': enigma.plugboard = optarg; break;
//        case 'p': rotorpos = optarg; break;
        case 'u': if (!load_reflector_config(&enigma, optarg)) print_usage(argv[0]); break;
        case 'w': if (!load_rotor_config(&enigma, optarg)) print_usage(argv[0]); break;
        default: print_usage(argv[0]); exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Print usage information for the Bombe CLI.
 *
 * This function prints the command line options and their descriptions.
 *
 * @param argv0 The name of the program, typically `argv[0]`.
 *
 */
static void print_usage(const char* argv0) {
    fprintf(stderr, "Usage: %s [-p positions] [-s plugboard] [-u reflector] [-w rotors] [-t threadCount] [-c plaintext] [-C position] method [file]\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c plaintext   Set known plaintext to use for the attack\n");
    fprintf(stderr, "  -C position    Set the position of known plaintext\n");
    fprintf(stderr, "  -p positions   Set the initial position of the rotors (e.g., 'ABC')\n");
    fprintf(stderr, "  -s plugboard   Set the plugboard (Steckerbrett) configuration (e.g., 'ABCDEF')\n");
    fprintf(stderr, "  -t threadCount Set the number of threads to use for processing (default: 1)\n");
    fprintf(stderr, "  -u reflector   Set the reflector (Umkehrwalze) configuration (e.g., 'B')\n");
    fprintf(stderr, "  -w rotors      Set the rotor (Walzen) configuration (e.g., 'I II III')\n");
    fprintf(stderr, "Available rotors: I, II, III, IV, V, VI, VII, VIII\n");
    fprintf(stderr, "Available reflectors: A, B, C\n");
    fprintf(stderr, "Methods:\n");
    fprintf(stderr, "  brute          Use Brute Force analysis for cryptanalysis\n");
    fprintf(stderr, "  ioc            Use Index of Coincidence for cryptanalysis\n");
    fprintf(stderr, "  bigram         Use Bigram analysis for cryptanalysis\n");
    fprintf(stderr, "  trigram        Use Trigram analysis for cryptanalysis\n");
    fprintf(stderr, "  quadgram       Use Quadgram analysis for cryptanalysis\n");
    exit(EXIT_FAILURE);
}
