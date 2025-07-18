#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/enigma.h"
#include "lib/rotors.h"
#include "lib/reflectors.h"

static void bombe(char*, char*, int);
static int load_reflector_config(enigma_t*, const char*);
static int load_rotor_config(enigma_t*, char*);
static int load_rotor_positions(enigma_t*, char*);
static void print_usage(const char*);

int main(int argc, char* argv[]) {
    int opt;
    enigma_t enigma;

    init_default_enigma(&enigma);

    // Parse command line options
    // Parameters look a bit too similar, maybe we can have better names?
    while ((opt = getopt(argc, argv, "s:p:u:w:")) != -1) {
        switch (opt) {
        case 's': enigma.plugboard = optarg; break;
        case 'p': if (!load_rotor_positions(&enigma, optarg)) print_usage(argv[0]); break;
        case 'u': if (!load_reflector_config(&enigma, optarg)) print_usage(argv[0]); break;
        case 'w': if (!load_rotor_config(&enigma, optarg)) print_usage(argv[0]); break;
        default: print_usage(argv[0]); exit(EXIT_FAILURE);
        }
    }

    // Main loop
    int c;
    while ((c = fgetc(stdin)) != EOF) {
        if (!isalpha(c)) {
            fputc(c, stdout);
            continue;
        }

        char output = encode(&enigma, c);
        if (isupper(c)) {
            fputc(output, stdout);
        } else {
            fputc(tolower(output), stdout);
        }
    }
    printf("\n");

    return EXIT_SUCCESS;
}

/**
 * @brief Load the rotor configuration from its string identifier.
 *
 * This function searches through the available reflectors and sets the
 * `reflector` field of the `enigma_t` to the one matching the provided identifier.
 * It assumes the memory for the reflector has already been allocated.
 *
 * @param enigma Pointer to the `enigma_t`.
 * @param s The string identifier for the reflector.
 * @return 1 if the reflector was found and loaded, 0 otherwise.
 */
static int load_reflector_config(enigma_t* enigma, const char* s) {
    for (int i = 0; i < 3; i++) {
        if (!strcmp(enigma_reflectors[i]->name, s)) {
            memcpy(enigma->reflector, enigma_reflectors[i], sizeof(reflector_t));
            return 1;
        }
    }

    fprintf(stderr, "Unknown reflector: %s\n", s);
    return 0;
}

static int load_rotor_config(enigma_t* enigma, char* s) {
    enigma->rotor_count = 0;
    int numRotors = 8;

    char* token = strtok(s, " ");
    while (token != NULL) {
        for (int i = 0; i < numRotors; i++) {
            if (!strcmp(enigma_rotors[i]->name, token)) {
                enigma->rotors[enigma->rotor_count++] = *enigma_rotors[i];
                break;
            }

            if (i == numRotors - 1) {
                fprintf(stderr, "Unknown rotor: %s\n", token);
                return 0;
            }
        }

        token = strtok(NULL, " ");
    }

    return 1;
}

static int load_rotor_positions(enigma_t* enigma, char* s) {
    if (enigma->rotor_count == 0) {
        fprintf(stderr, "No rotors loaded\n");
        return 0;
    }

    char* token = strtok(s, " ");
    for (int i = 0; i < enigma->rotor_count && token != NULL; i++) {
        enigma->rotors[i].idx = toupper(token[0]) - 'A';
        token = strtok(NULL, " ");
    }

    return 1;
}

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
