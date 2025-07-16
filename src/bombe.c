#include "bombe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

#define ALPHA_SIZE 26

static void bombe_print_partial_configuration(enigma_t *enigma);
static void bombe_process_chunk(bombe_t *, enigma_t *, const char *, int);
static void bombe_process_single(bombe_t *, enigma_t *, const char *, int);

/**
 * @brief Initializes the Bombe structure with cribs and Enigma configuration.
 *
 * @param bombe The bombe structure to initialize
 * @param cribStrings Array of crib strings to be used
 * @param cribIndices Array of crib indices for each crib string
 * @param cribLength Length of the crib array
 * @param enigma Pointer to the Enigma configuration
 */
void bombe_init(bombe_t *bombe, char **cribStrings, int *cribIndices, int numCribs) {
    for (int i = 0; i < numCribs; i++) {
        bombe->crib[i].s = cribStrings[i];
        bombe->crib[i].index = cribIndices[i];
    }
    bombe->numCribs = numCribs;
}

/**
 * @brief Runs the Bombe algorithm with the provided cribs and configuration.
 * @param bombe The bombe structure containing cribs and configuration
 * @param ciphertext The ciphertext to analyze
 * @param offset Potential offset for crib matching
 * @param threadCount Number of threads to use for processing
 */
void bombe_run(bombe_t *bombe, const char *ciphertext) {
    int rotor_count = 8;
    int reflector_count = 3;
    int plugboard_count = 0;
    int ciphertextLength = strlen(ciphertext);

    printf("Running Bombe...\n");
    printf("Crib:\n");
    for (int i = 0; i < bombe->numCribs; i++) {
        printf(" * %d %s\n", bombe->crib[i].index, bombe->crib[i].s);
    }
}

/**
 * @brief Process a rotor configuration chunk.
 *
 * Here we take a rotor, reflector, and plugboard configuration, minus rotor positions,
 * and loop through all possible rotor positions and check if any match our crib.
 *
 * This loop runs 26^3=17576 times.
 */
static void bombe_process_chunk(bombe_t *bombe, enigma_t *enigma, const char *ciphertext, int ciphertextLength) {
    // Placeholder for processing each chunk of ciphertext
    printf("Processing chunk with rotors (%s, %s, %s), reflector %s, plugboard %s\n",
           enigma->rotors[0].name, enigma->rotors[1].name, enigma->rotors[2].name,
           enigma->reflector->name, enigma->plugboard);

    for (int i = 0; i < ALPHA_SIZE; i++) {
        for (int j = 0; j < ALPHA_SIZE; j++) {
            for (int k = 0; k < ALPHA_SIZE; k++) {
                // Set rotor positions based on i, j, k
                enigma->rotors[2].idx = i;
                enigma->rotors[1].idx = j;
                enigma->rotors[0].idx = k;

                // Run Bombe algorithm with the current configuration
                bombe_process_single(bombe, enigma, ciphertext, ciphertextLength);
            }
        }
    }
}

/**
 * @brief Check the given enigma configuration against the ciphertext.
 */
static void bombe_process_single(bombe_t *bombe, enigma_t *enigma, const char *ciphertext, int ciphertextLength) {
    int matching = -1;
    int matchedLength = 0;

    for (int i = 0; i < ciphertextLength; i++) {
        if (i > bombe->lastCribIndex && matching == -1) {
            return; // No matches found for this configuration
        }

        char decrypted = encode(enigma, ciphertext[i]);

        if (matching == -1) {
            for (int j = 0; j < bombe->numCribs; j++) {
                if (bombe->crib[j].index == i) {
                    matching = j;
                    break;
                }
            }
        }

        if (matching != -1) {
            if (decrypted == bombe->crib[matching].s[matchedLength]) {
                matchedLength++;
                if (matchedLength == bombe->crib[matching].length) {
                    break;
                }
            } else {
                matching = -1;
                matchedLength = 0;
            }
        }
    }

    printf("Potential configuration:\n * Rotors: %s (%d) %s (%d) %s (%d)\n * Reflector: %s\n * Plugboard: %s\n",
            enigma->rotors[0].name, enigma->rotors[0].idx,
            enigma->rotors[1].name, enigma->rotors[1].idx,
            enigma->rotors[2].name, enigma->rotors[2].idx,
            enigma->reflector->name, enigma->plugboard ? enigma->plugboard : "None");
}

static void bombe_print_partial_configuration(enigma_t *enigma) {
    printf("Using partial Enigma configuration:\n");
    if (enigma->rotors) {
        printf("Rotors: ");
        for (int i = 0; i < enigma->rotor_count; i++) {
            if (enigma->rotors[i].name) {
                printf("%s ", enigma->rotors[i].name);
            } else {
                printf("X ");
            }
        }
        printf("\n");
    }

    if (enigma->plugboard) {
        printf("Plugboard: %s\n", enigma->plugboard);
    }

    if (enigma->reflector) {
        printf("Reflector: %s\n", enigma->reflector->name);
    }
}
