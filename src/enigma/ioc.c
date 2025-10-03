#include "ioc.h"

#include "enigma.h"
#include "io.h"
#include "threads.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void* rotor_thread_main(void*);

/**
 * @brief Score text using Index of Coincidence.
 *
 * @param text The text to score.
 * @param len The length of the text.
 * @param placeholder Unused parameter, present for qsort function signature consistency.
 */
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
 */
void enigma_crack_rotors_ioc(enigma_crack_config_t* config) {
    enigma_crack_multithreaded(config, (void* (*)(void*))rotor_thread_main);
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
void enigma_crack_rotor_positions_ioc(enigma_crack_config_t* config) {
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

static void* rotor_thread_main(void* args) {
#define THREADNUM ((int*)args)[1]
#define MYENIGMA enigma_enigmas[THREADNUM]


    if (THREADNUM == 0) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            memcpy(&MYENIGMA.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
            for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
                if (i == j) {
                    continue;
                }
                memcpy(&MYENIGMA.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
                for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                    memcpy(&MYENIGMA.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));
                    if (j == k) {
                        continue;
                    }

                    enigma_spawn(0, 0);
                }
            }
        }
    } else {
        char* decrypted = &enigma_plaintexts[THREADNUM];
        char buf[80];
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, decrypted, enigma_global_cfg->ciphertextLen);

        float score = enigma_ioc_score(decrypted, enigma_global_cfg->ciphertextLen, NULL);
        if (score > enigma_global_cfg->minScore && score < enigma_global_cfg->maxScore) {
            enigma_print_config(&MYENIGMA, buf);
            printf("%.6f\t%s\t%s\n", score, buf, decrypted);
        }
    }
    enigma_freeThreads[THREADNUM] = 1;
#undef THREADNUM
#undef MYENIGMA
    return NULL;
}
