#include "ic.h"

#include "enigma.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const float enigma_ic_english = 0.067;
const float enigma_ic_german = 0.078;

static void crack_plugboard(enigma_t *, const char *, char *, int, float);
static void crack_reflector(enigma_t *, const char *, char *, int, float);
static void crack_rotor_indexes(enigma_t *, const char *, char *, int, float);
static void crack_rotor_positions(enigma_t *, const char*, char*, int, float);
static void encode_string(enigma_t*, const char*, char*, int);
void find_freq(int *, const char*, int len);
float find_ic(float, int*, int);

/**
 * @brief Crack the Enigma machine using Index of Coincidence (IC).
 *
 * @param ciphertext The ciphertext to analyze.
 * @param length The length of the ciphertext.
 * @param target The target index of coincidence (e.g., enigma_ic_english).
 */
void enigma_crack(const char *ciphertext, int length, float target) {
    int freq[26] = {0};
    char *plaintext = malloc(length + 1);
    enigma_t enigma;

    enigma_init_default_config(&enigma);
    enigma.plugboard = "";

    crack_rotor_positions(&enigma, ciphertext, plaintext, length, target);
    crack_rotor_indexes(&enigma, ciphertext, plaintext, length, target);

    for (int i = 0; i < enigma.rotor_count; i++) {
        printf("Rotor %d: %s\n", i + 1, enigma.rotors[i].name);
    }

    printf("Plaintext: %s\n", plaintext);
}

static void crack_plugboard(enigma_t *enigma, const char *ciphertext, char *plaintext, int length, float target) {
    int freq[26] = {0};
    float cur = 0.0f;
    float prev = 0.0f;
}

static void crack_reflector(enigma_t *enigma, const char *ciphertext, char *plaintext, int length, float target) {
    int freq[26] = {0};
    float cur = 0.0f;
    float prev = 0.0f;

    for (int i = 0; i < REFLECTOR_COUNT; i++) {
        enigma->reflector = enigma_reflectors[i];
        encode_string(enigma, ciphertext, plaintext, length);
        find_freq(freq, plaintext, length);
        prev = cur;
        cur = find_ic(target, freq, length);

        if (cur > prev) {
            printf("Found reflector: %s\n", enigma->reflector->name);
            return;
        }
    }
}


static void crack_rotor_indexes(enigma_t *enigma, const char *ciphertext, char *plaintext, int length, float target) {
    int freq[26] = {0};
    float cur = 0.0f;
    float prev = 0.0f;

    for (int i = 0; i < enigma->rotor_count; i++) {
        for (int j = 0; j < ALPHA_SIZE; j++) {
            enigma->rotors[i].idx = j;
            encode_string(enigma, ciphertext, plaintext, length);
            find_freq(freq, plaintext, length);
            prev = cur;
            cur = find_ic(target, freq, length);

            if (cur > prev) {
                printf("Found rotor index %d: %c\n", i + 1, j + 'A');
                break;
            }
        }
    }
}

/**
 * @brief Crack rotor positions based on the ciphertext and target IC.
 *
 * This function iterates through all possible rotor positions and calculates the
 * Index of Coincidence (IC) for each configuration. It compares the calculated IC
 * with the target IC and stores the rotor positions that yield a higher IC.
 *
 * @param ciphertext The ciphertext to analyze.
 * @param plaintext Buffer to store the decrypted plaintext.
 * @param length The length of the ciphertext.
 * @param target The target index of coincidence (e.g., enigma_ic_english).
 */
static void crack_rotor_positions(enigma_t *enigma, const char *ciphertext, char *plaintext, int length, float target) {
    float cur = 0.0f;
    float prev = 0.0f;
    int freq[26] = {0};
    int rotorsFound = 0;

    for (int i = 0; i < enigma->rotor_count; i++) {
        for (int j = 0; j < ROTOR_COUNT; j++) {
            for (int k = 0; k < rotorsFound; k++) {
                if (!strcmp(enigma->rotors[k].name, enigma_rotors[j]->name)) {
                    j++;
                }
            }
            encode_string(enigma, ciphertext, plaintext, length);
            find_freq(freq, plaintext, length);
            prev = cur;
            cur = find_ic(target, freq, length);

            if (cur > prev) {
                enigma->rotors[rotorsFound++] = *enigma_rotors[j];
                if (rotorsFound == enigma->rotor_count) {
                    printf("Found rotor configuration: %s %s %s\n", enigma->rotors[0].name, enigma->rotors[1].name, enigma->rotors[2].name);
                    return;
                }
            }
        }
    }
}

static void encode_string(enigma_t* enigma, const char* input, char* output, int length) {
    for (int i = 0; i < length; i++) {
        output[i] = enigma_encode(enigma, input[i]);
    }
    output[length] = '\0';
}

float find_ic(float ic, int* freq, int length) {
    for (int i = 0; i < 26; i++) {
        ic += (float)freq[i] * (freq[i] - 1);
    }
    return ic / (float)(length * (length - 1));
}

void find_freq(int *dst, const char *text, int len) {
    for (int i = 0; i < len; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }

        dst[text[i] - 'A']++;
    }
}
