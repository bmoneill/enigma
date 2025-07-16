#include "bombe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

#define ALPHA_SIZE 26

static void bombe_print_partial_configuration(enigma_t *enigma);
static void bombe_process_chunk(bombe_t *, enigma_t *, const char *, int, char *);
static void bombe_process_single(bombe_t *, enigma_t *, const char *, int, char *, const char *);

/**
 * @brief Initializes the Bombe structure with cribs and Enigma configuration.
 *
 * @param bombe The bombe structure to initialize
 * @param cribStrings Array of crib strings to be used
 * @param cribIndices Array of crib indices for each crib string
 * @param numCribs Number of crib strings (length of cribStrings and cribIndices)
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
 */
void bombe_run(bombe_t *bombe, const char *ciphertext) {
    int rotor_count = 3;
    int reflector_count = 3;
    int plugboard_count = 0;
    int ciphertextLength = strlen(ciphertext);

    printf("Running Bombe...\n");
    printf("Crib:\n");
    for (int i = 0; i < bombe->numCribs; i++) {
        printf(" - Index: %d, String: %s\n", bombe->crib[i].index, bombe->crib[i].s);
    }

    enigma_t enigma;
    enigma.rotors = malloc(rotor_count * sizeof(rotor_t));
    enigma.reflector = malloc(sizeof(reflector_t));
    enigma.plugboard = NULL;
    enigma.rotor_count = 3;
    enigma.rotor_flag = 0;
    memcpy(&enigma.rotors[2], &rotor_I, sizeof(rotor_t));
    memcpy(&enigma.rotors[1], &rotor_II, sizeof(rotor_t));
    memcpy(&enigma.rotors[0], &rotor_III, sizeof(rotor_t));
    memcpy(enigma.reflector, &UKW_B, sizeof(reflector_t));

    char *plaintext = malloc(ciphertextLength + 1);

    bombe_process_chunk(bombe, &enigma, ciphertext, ciphertextLength, plaintext);
}

/**
 * @brief Process a rotor configuration chunk.
 *
 * Here we take a rotor, reflector, and plugboard configuration, minus rotor positions,
 * and loop through all possible rotor positions and check if any match our crib.
 *
 * This loop runs 26^3=17576 times.
 *
 * @param bombe The bombe structure containing cribs and configuration
 * @param enigma The Enigma configuration to use for processing
 * @param ciphertext The ciphertext to analyze
 * @param ciphertextLength The length of the ciphertext
 * @param plaintext Buffer to store the decrypted plaintext (to avoid mallocing each time)
 */
static void bombe_process_chunk(bombe_t *bombe, enigma_t *enigma, const char *ciphertext, int ciphertextLength, char *plaintext) {
    printf("Processing chunk with rotors (%s, %s, %s), reflector %s\n",
           enigma->rotors[0].name, enigma->rotors[1].name, enigma->rotors[2].name,
           enigma->reflector->name);

    char configString[256];

    for (int i = 0; i < ALPHA_SIZE; i++) {
        for (int j = 0; j < ALPHA_SIZE; j++) {
            for (int k = 0; k < ALPHA_SIZE; k++) {
                enigma->rotors[2].idx = i;
                enigma->rotors[1].idx = j;
                enigma->rotors[0].idx = k;
                sprintf(configString, "Rotors: %s (%c)  %s (%c), %s (%c) | Reflector: %s",
                        enigma->rotors[0].name, enigma->rotors[0].idx + 'A',
                        enigma->rotors[1].name, enigma->rotors[1].idx + 'A',
                        enigma->rotors[2].name, enigma->rotors[2].idx + 'A',
                        enigma->reflector->name);

                bombe_process_single(bombe, enigma, ciphertext, ciphertextLength, plaintext, configString);
            }
        }
    }
}

/**
 * @brief Check the given enigma configuration against the ciphertext.
 *
 * @param bombe The bombe structure containing cribs and configuration
 * @param enigma The Enigma configuration to use for processing
 * @param ciphertext The ciphertext to analyze
 * @param ciphertextLength The length of the ciphertext
 * @param plaintext Buffer to store the decrypted plaintext
 * @param configString String representation of the current Enigma configuration
 */
static void bombe_process_single(bombe_t *bombe, enigma_t *enigma, const char *ciphertext, int ciphertextLength, char *plaintext, const char *configString) {
    int matching = -1;
    int matchedLength = 0;

    for (int i = 0; i < ciphertextLength; i++) {
        if (i > bombe->lastCribIndex && matching == -1) {
            return; // No matches found for this configuration
        }

        char decrypted = encode(enigma, ciphertext[i]);
        plaintext[i] = decrypted;

        if (matching == -1) {
            for (int j = 0; j < bombe->numCribs; j++) {
                if (bombe->crib[j].index == i && bombe->crib[j].s[0] == decrypted) {
                    matching = j;
                    matchedLength++;
                    break;
                }
            }
        } else {
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

    printf("%s | Plaintext: %s\n", configString, plaintext);
}
