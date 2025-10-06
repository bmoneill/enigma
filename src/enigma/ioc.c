#include "ioc.h"

#include "enigma.h"
#include "io.h"
#include "threads.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THREADNUM ((int*)args)[1]
#define MYENIGMA enigma_enigmas[THREADNUM]

static void* plugboard_thread_main(void* args);
static void* positions_thread_main(void* args);
static void* reflector_thread_main(void* args);
static void* rotor_thread_main(void*);

/**
 * @brief Crack rotor configuration using Index of Coincidence.
 *
 * This is a wrapper for enigma_crack_multithreaded that uses the rotor_thread_main function.
 *
 * @param config Pointer to the cracking configuration structure.
 */
void enigma_crack_rotors_ioc(enigma_crack_config_t* config) {
    enigma_crack_multithreaded(config, (void* (*)(void*))rotor_thread_main);
}

/**
 * @brief Crack rotor positions using Index of Coincidence.
 *
 * This function attempts to determine the rotor positions used in the ciphertext's Enigma key
 * by evaluating different rotor combinations and scoring the resulting plaintext
 * using the Index of Coincidence method.
 *
 * It is recommended that a suspected rotor configuration be set in the enigma structure.
 *
 * @param config Pointer to the cracking configuration structure.
 */
void enigma_crack_rotor_positions_ioc(enigma_crack_config_t* config) {
    enigma_crack_multithreaded(config, (void* (*)(void*))positions_thread_main);
}

/**
 * @brief Crack reflector using Index of Coincidence.
 *
 * This function attempts to determine the reflector used in the ciphertext's Enigma key
 * by and scoring the resulting plaintext from all reflectors using the Index of Coincidence method.
 *
 * @param config Pointer to the cracking configuration structure.
 */
void enigma_crack_reflector_ioc(enigma_crack_config_t* config) {
    enigma_crack_multithreaded(config, (void* (*)(void*))reflector_thread_main);
}

/**
 * @brief Crack plugboard using Index of Coincidence.
 *
 * This function attempts to determine the plugboard settings used in the ciphertext's Enigma key
 * by and scoring the resulting plaintext from all plugboard settings using the Index of Coincidence
 * method.
 *
 * @param config Pointer to the cracking configuration structure.
 */
void enigma_crack_plugboard_ioc(enigma_crack_config_t* config) {
    enigma_crack_multithreaded(config, (void* (*)(void*))plugboard_thread_main);
}

/**
 * @brief Analyze decrypted text using Index of Coincidence.
 *
 * This function scores the decrypted text using the Index of Coincidence method
 * and prints the configuration and score if it falls within the specified range.
 *
 * @param threadnum The thread number corresponding to the decrypted text to analyze.
 */
void enigma_ioc_analyze(int threadnum) {
    float score = enigma_ioc_score(&enigma_plaintexts[threadnum], enigma_global_cfg->ciphertextLen, NULL);
    if (score > enigma_global_cfg->minScore && score < enigma_global_cfg->maxScore) {
        char buf[80];
        enigma_print_config(&enigma_enigmas[threadnum], buf);
        printf("%.6f\t%s\t%s\n", score, buf, &enigma_plaintexts[threadnum]);
    }
}

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
 * @brief Thread main function for plugboard cracking.
 *
 * This function iterates through all possible plugboard settings with up to
 * enigma_global_cfg->maxPlugboardSettings pairs and evaluates
 * the resulting plaintext using the Index of Coincidence method.
 *
 * It is recommended that this be the last step in a cracking process, as it is the most
 * computationally expensive. Suspected rotor configuration, positions, and reflector
 * settings should be set in the global configuration's enigma structure.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* plugboard_thread_main(void* args) {
    if (THREADNUM == 0) {
        enigma_spawn(0, THREADNUM);

        for (int i = 1; i < enigma_global_cfg->maxPlugboardSettings; i++) {
            for (int j = 0; j < i; j++) {
                for (int a = 0; a < ENIGMA_ALPHA_SIZE; a++) {
                    for (int b = 0; b < ENIGMA_ALPHA_SIZE; b++) {
                        if (a == b) continue;

                        MYENIGMA.plugboard[j * 2] = 'A' + a;
                        MYENIGMA.plugboard[j * 2 + 1] = 'A' + b;

                        enigma_spawn(0, THREADNUM);
                    }
                }
            }
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        enigma_ioc_analyze(THREADNUM);
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

/**
 * @brief Thread main function for rotor position cracking.
 *
 * This function iterates through all possible rotor positions
 * and evaluates the resulting plaintext using the Index of Coincidence method.
 *
 * It is recommended that a suspected rotor configuration be set in global configuration's enigma
 * structure.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* positions_thread_main(void* args) {
    if (THREADNUM == 0) {
        for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
            MYENIGMA.rotors[0].idx = i;
            for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
                MYENIGMA.rotors[1].idx = j;
                for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                    MYENIGMA.rotors[2].idx = k;
                    enigma_spawn(0, 0);
                }
            }
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        enigma_ioc_analyze(THREADNUM);
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

/**
 * @brief Thread main function for reflector cracking.
 *
 * This function iterates through all possible reflectors
 * and evaluates the resulting plaintext using the Index of Coincidence method.
 *
 * It is recommended that a suspected rotor configuration and positions be set in the
 * global configuration's enigma structure.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* reflector_thread_main(void* args) {
    if (THREADNUM == 0) {
        for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
            memcpy(&MYENIGMA.reflector, &enigma_reflectors[i], sizeof(enigma_reflector_t));
            enigma_spawn(0, 0);
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        enigma_ioc_analyze(THREADNUM);
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

/**
 * @brief Thread main function for rotor cracking.
 *
 * This function iterates through all possible rotor combinations
 * and evaluates the resulting plaintext using the Index of Coincidence method.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
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
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        enigma_ioc_analyze(THREADNUM);
    }
    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

#undef THREADNUM
#undef MYENIGMA
