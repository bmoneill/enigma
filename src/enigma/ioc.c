#include "ioc.h"

#include "enigma.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float enigma_ioc_score(const char* text, int len, void* placeholder) {
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
 * @param config Pointer to the cracking configuration structure.
 *
 * @todo Fix for new enigma_crack_config_t structure.
 */
void enigma_crack_rotors_ioc(enigma_crack_config_t* config) {
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
}

/**
 * @brief Crack rotor positions using Index of Coincidence.
 *
 * Currently unimplemented.
 *
 * This function attempts to determine the rotor positions used in the ciphertext's Enigma key
 * by evaluating different rotor combinations and scoring the resulting plaintext
 * using the Index of Coincidence method.
 *
 * It is recommended that a suspected rotor configuration be set in the enigma structure.
 *
 * @param config Pointer to the cracking configuration structure.
 *
 * @todo Implement
 */
void enigma_crack_rotor_positions_ioc(enigma_crack_config_t*) {
    // TODO implement
}

/**
 * @brief Crack reflector using Index of Coincidence.
 *
 * Currently unimplemented.
 *
 * This function attempts to determine the reflector used in the ciphertext's Enigma key
 * by and scoring the resulting plaintext from all reflectors using the Index of Coincidence method.
 *
 * @param config Pointer to the cracking configuration structure.
 *
 * @todo Implement
 */
void enigma_crack_reflector_ioc(enigma_crack_config_t* config) {
    // TODO implement
}

/**
 * @brief Crack plugboard using Index of Coincidence.
 *
 * Currently unimplemented.
 *
 * This function attempts to determine the plugboard settings used in the ciphertext's Enigma key
 * by and scoring the resulting plaintext from all plugboard settings using the Index of Coincidence
 * method.
 *
 * @param config Pointer to the cracking configuration structure.
 *
 * @todo Implement
 */
void enigma_crack_plugboard_ioc(enigma_crack_config_t* config) {
    // TODO implement
}
