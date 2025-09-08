#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma/common.h"
#include "enigma/crack.h"
#include "enigma/enigma.h"

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {
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
    fprintf(stderr, "Usage: %s method [target] [options] [file]\n", argv0);
    fprintf(stderr, "Methods:\n");
    fprintf(stderr, "  bombe            Use a pseudo-Bombe for cryptanalysis (-c required)\n");
    fprintf(stderr, "  brute            Use Brute Force for cryptanalysis\n");
    fprintf(stderr, "  ioc              Use Index of Coincidence for cryptanalysis (target required)\n");
    fprintf(stderr, "  bigram           Use Bigram analysis for cryptanalysis (target required)\n");
    fprintf(stderr, "  trigram          Use Trigram analysis for cryptanalysis (target required)\n");
    fprintf(stderr, "  quadgram         Use Quadgram analysis for cryptanalysis (target required)\n\n");
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
    fprintf(stderr, "    -l language    Language ('english' or 'german')\n");
    fprintf(stderr, "    -m float       Minimum score threshold (for n-gram and IOC methods)\n");
    fprintf(stderr, "    -M float       Maximum score threshold (for n-gram and IOC methods)\n");
    fprintf(stderr, "    -n file        n-gram bank to load\n\n");
    fprintf(stderr, "    -S count       Set the maximum number of plugboard settings (e.g. '10')\n");
    fprintf(stderr, "    -t threadCount Number of threads to use\n\n");
    fprintf(stderr, "Other Options:\n");
    fprintf(stderr, "    -h             Show this help message\n");
    fprintf(stderr, "    -v             Show verbose output\n\n");
    fprintf(stderr, "A file can be provided as the last argument to read the ciphertext from a file.\n");
    fprintf(stderr, "If no file is provided, the ciphertext will be read from standard input.\n\n");
    fprintf(stderr, "Available rotors: I, II, III, IV, V, VI, VII, VIII\n");
    fprintf(stderr, "Available reflectors: A, B, C\n");
    exit(EXIT_FAILURE);
}

static enigma_crack_config_t *parse_arguments(int argc, char* argv[]) {
    int opt;

    enigma_crack_config_t *config = calloc(1, sizeof(enigma_crack_config_t));
    if (!config) {
        fprintf(stderr, "Failed to allocate memory for config\n");
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "w:p:u:s:c:C:l:m:M:n:S:t:v")) != -1) {
        switch (opt) {
        case 'w':
            if (!enigma_load_rotor_config(&config->enigma, optarg)) {
                fprintf(stderr, "Error: Invalid rotor configuration.\n");
                print_usage(argv[0]);
            }
            break;
        case 'p':
            if (!enigma_load_rotor_positions(&config->enigma, optarg)) {
                fprintf(stderr, "Error: Invalid rotor positions.\n");
                print_usage(argv[0]);
            }
            break;
        case 'u':
            if (!enigma_load_reflector_config(&config->enigma, optarg)) {
                fprintf(stderr, "Error: Invalid reflector configuration.\n");
                print_usage(argv[0]);
            }
            break;
        case 's': config->enigma.plugboard = optarg; break;
        case 'c': config->plaintext = optarg; break;
        case 'C': config->plaintextPos = atoi(optarg); break;
        case 'l': load_language(config, optarg); break;
        case 'm': config->minScore = atof(optarg); break;
        case 'M': config->maxScore = atof(optarg); break;
        case 'n': load_ngrams(&config->ngrams, &config->ngramCount, optarg); break;
        case 'S': config->maxPlugboardSettings = atoi(optarg); break;
        case 't': config->threadCount = atoi(optarg); break;
        case 'v': enigma_verbose = true; break;
        default: print_usage(argv[0]);
        }
    }
    return config;
}
