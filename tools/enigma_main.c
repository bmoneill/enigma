#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

static void print_usage(const char*);

int main(int argc, char* argv[]) {
    int opt;
    enigma_t enigma;

    enigma_init_default_config(&enigma);

    // Parse command line options
    char *rotorpos = NULL;
    while ((opt = getopt(argc, argv, "s:p:u:w:")) != -1) {
        switch (opt) {
        case 's': enigma.plugboard = optarg; break;
        case 'p': rotorpos = optarg; break;
        case 'u': if (!load_reflector_config(&enigma, optarg)) print_usage(argv[0]); break;
        case 'w': if (!load_rotor_config(&enigma, optarg)) print_usage(argv[0]); break;
        default: print_usage(argv[0]); exit(EXIT_FAILURE);
        }
    }

    if (rotorpos && !load_rotor_positions(&enigma, rotorpos)) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Main loop
    int c;
    int lastc = 0;
    while ((c = fgetc(stdin)) != EOF) {
        if (!isalpha(c)) {
            fputc(c, stdout);
            lastc = c;
            continue;
        }

        char output = enigma_encode(&enigma, toupper(c));
        if (isupper(c)) {
            fputc(output, stdout);
        } else {
            fputc(tolower(output), stdout);
        }
        lastc = c;
    }

    if (lastc != '\n') {
        fputc('\n', stdout);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Print usage information for the Enigma CLI.
 *
 * This function prints the command line options and their descriptions.
 *
 * @param argv0 The name of the program, typically `argv[0]`.
 */
static void print_usage(const char* argv0) {
    fprintf(stderr, "Usage: %s [-p positions] [-s plugboard] [-u reflector] [-w rotors]\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -p positions    Set the initial position of the rotors (e.g., 'ABC')\n");
    fprintf(stderr, "  -s plugboard   Set the plugboard (Steckerbrett) configuration (e.g., 'ABCDEF')\n");
    fprintf(stderr, "  -u reflector   Set the reflector (Umkehrwalze) configuration (e.g., 'B')\n");
    fprintf(stderr, "  -w rotors      Set the rotor (Walzen) configuration (e.g., 'I II III')\n");
    fprintf(stderr, "Available rotors: I, II, III, IV, V, VI, VII, VIII\n");
    fprintf(stderr, "Available reflectors: A, B, C\n");
    exit(EXIT_FAILURE);
}
