#include "ioc.h"

#include "enigma.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void crack_plugboard(enigma_t*, const char*, char*, int, float);
static void crack_reflector(enigma_t*, const char*, char*, int, float);
static void crack_rotor_positions(enigma_t*, const char*, char*, int, float);
static void encode_string(enigma_t*, const char*, char*, int);
static void find_freq(int*, const char*, int len);
static float find_ic(float, int*, int);

float enigma_ic_score(const char* text, int len, void* placeholder) {
    int freq[26] = { 0 };
    float total = 0.0f;

    for (int i = 0; i < len; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }
        freq[text[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        total += (float)freq[i] * (freq[i] - 1);
    }
    return total / (float)(len * (len - 1));
}

/**
 * @brief Crack rotor configuration using Index of Coincidence.
 *
 * This function attempts to determine the rotors used in the ciphertext's Enigma key
 * by evaluating different rotor combinations and scoring the resulting plaintext
 * using the Index of Coincidence method.
 *
 * @param enigma Pointer to the Enigma machine configuration. This may contain known settings if applicable.
 * @param ciphertext The ciphertext to be analyzed.
 * @param plaintext Buffer to store the resulting plaintext. Must be at least `len + 1` bytes.
 * @param len Length of the ciphertext.
 * @param count The number of top results to keep track of.
 *
 * @return An array of `crack_result_t` structures containing the top rotor configurations and their scores.
 */
crack_result_t *crack_rotors(enigma_t* enigma, const char* ciphertext, char* plaintext, int len, int count) {
    crack_result_t* results = malloc(sizeof(crack_result_t) * count);
    char* temp_plaintext = malloc(len + 1);

    int result_count = 0;
    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        memcpy(&enigma->rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
        for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
            if (i == j) {
                continue;
            }
            memcpy(&enigma->rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
            for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                if (k == i || k == j) {
                    continue;
                }
                memcpy(&enigma->rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));

                encode_string(enigma, ciphertext, temp_plaintext, len);
                float score = enigma_ic_score(temp_plaintext, len, NULL);
                if (result_count < count) {
                    results[result_count].enigma = malloc(sizeof(enigma_t));
                    memcpy(results[result_count].enigma, enigma, sizeof(enigma_t));
                    results[result_count].score = score;
                    result_count++;
                }
                else {
                    // Find the lowest score in results
                    int min_index = 0;
                    for (int r = 1; r < result_count; r++) {
                        if (results[r].score < results[min_index].score) {
                            min_index = r;
                        }
                    }
                    if (score > results[min_index].score) {
                        free(results[min_index].enigma);
                        results[min_index].enigma = malloc(sizeof(enigma_t));
                        memcpy(results[min_index].enigma, enigma, sizeof(enigma_t));
                        results[min_index].score = score;
                    }
                }
            }
        }
    }
    free(temp_plaintext);
    return results;
}
