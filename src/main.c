#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

static void load_default_enigma(enigma_t *);
static int load_reflector_config(enigma_t *, const char *);
static int load_rotor_config(enigma_t *, char *);
static int load_rotor_positions(enigma_t *, char *);
static void print_usage(const char *);

int main(int argc, char *argv[]) {
    int opt;
    int verbose = 0;
    enigma_t enigma;

    load_default_enigma(&enigma);

    // Parse command line options
    // Parameters look a bit too similar, maybe we can have better names?
    while ((opt = getopt(argc, argv, "p:P:r:R:")) != -1) {
        switch (opt) {
        case 'p': enigma.plugboard = optarg; break;
        case 'P': if (!load_rotor_positions(&enigma, optarg)) print_usage(argv[0]); break;
        case 'r': if (!load_rotor_config(&enigma, optarg)) print_usage(argv[0]); break;
        case 'R': if (!load_reflector_config(&enigma, optarg)) print_usage(argv[0]); break;
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
        fputc(output, stdout);
    }
    printf("\n");

    free(enigma.rotors);
    free(enigma.reflector);
    return EXIT_SUCCESS;
}

/**
 * @brief Load the default configuration for the Enigma machine.
 *
 * Default config is as follows:
 * - Reflector: UKW-B
 * - Rotors: I, II, III
 * - Rotor positions: A, A, A
 * - Empty plugboard
 *
 * @param enigma Pointer to the `enigma_t` to be initialized.
 */
static void load_default_enigma(enigma_t *enigma) {
    enigma->reflector = malloc(sizeof(reflector_t));
    memcpy(enigma->reflector, &UKW_B, sizeof(reflector_t));
    enigma->rotor_count = 3;
    enigma->rotors = malloc(enigma->rotor_count * sizeof(rotor_t));
    enigma->rotors[0] = rotor_I;
    enigma->rotors[1] = rotor_II;
    enigma->rotors[2] = rotor_III;
    enigma->plugboard = NULL;
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
static int load_reflector_config(enigma_t *enigma , const char *s) {
    for (int i = 0; i < sizeof(enigma_reflectors) / sizeof(reflector_t *); i++) {
        if (!strcmp(enigma_reflectors[i]->name, s)) {
            memcpy(enigma->reflector, enigma_reflectors[i], sizeof(reflector_t));
            return 1;
        }
    }

    fprintf(stderr, "Unknown reflector: %s\n", s);
    return 0;
}

static int load_rotor_config(enigma_t *enigma, char *s) {
    enigma->rotor_count = 0;
    int numRotors = sizeof(enigma_rotors) / sizeof(rotor_t *);

    char *token = strtok(s, " ");
    while (token != NULL) {
        printf("Loading rotor: %s\n", token);
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

static int load_rotor_positions(enigma_t *enigma, char *s) {
    if (enigma->rotor_count == 0) {
        fprintf(stderr, "No rotors loaded\n");
        return 0;
    }

    char *token = strtok(s, " ");
    for (int i = 0; i < enigma->rotor_count && token != NULL; i++) {
        enigma->rotors[i].idx = toupper(token[0]) - 'A';
        token = strtok(NULL, " ");
    }

    return 1;
}

static void print_usage(const char *argv0) {
    fprintf(stderr, "Usage: %s [-p plugboard] [-r rotors] [-R reflector] [-P position] [-v]\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -p plugboard   Set the plugboard configuration (e.g., 'AB CD EF')\n");
    fprintf(stderr, "  -P position    Set the initial position of the rotors (e.g., 'ABC')\n");
    fprintf(stderr, "  -r rotors      Set the rotor configuration (e.g., 'I II III')\n");
    fprintf(stderr, "  -R reflector   Set the reflector configuration (e.g., 'B')\n");
    fprintf(stderr, "Available rotors: I, II, III, IV, V, VI, VII, VIII\n");
    fprintf(stderr, "Available reflectors: A, B, C\n");
    exit(EXIT_FAILURE);
}
