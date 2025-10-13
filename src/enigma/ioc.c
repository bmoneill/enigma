/**
 * @file enigma/ioc.c
 *
 * This file implements cracking functions using the Index of Coincidence method.
 */
#include "ioc.h"

#include "enigma.h"
#include "io.h"
#include "threads.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THREADNUM ((int*)args)[1]
#define MYENIGMA enigma_enigmas[THREADNUM]

static void* plugboard_thread_main(void*);
static void* positions_thread_main(void*);
static void* reflector_thread_main(void*);
static void* rotor_thread_main(void*);
static void  ioc_analyze(int);

/**
 * @brief Crack a specific rotor using Index of Coincidence.
 *
 * This function attempts to determine the rotor used in the specified position
 * of the Enigma machine by evaluating all possible rotors and scoring the resulting
 * plaintext using the Index of Coincidence method.
 *
 * It is recommended that suspected rotor positions, reflector, and plugboard settings
 * be set in the enigma structure.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param targetRotor The index of the rotor to crack (0-based).
 */
void enigma_crack_rotor_ioc(enigma_crack_config_t* cfg, int targetRotor) {
    enigma_t enigma;
    memcpy(&enigma, cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        memcpy(&enigma.rotors[targetRotor], enigma_rotors[i], sizeof(enigma_rotor_t));
        enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
        float score = enigma_ioc_score(plaintext, cfg);
        printf("Rotor %s: Score %.6f\n", enigma.rotors[targetRotor].name, score);
    }

    free(plaintext);
}

/**
 * @brief Crack the reflector using Index of Coincidence.
 *
 * This function attempts to determine the reflector used in the Enigma machine
 * by evaluating all possible reflectors and scoring the resulting plaintext
 * using the Index of Coincidence method.
 *
 * @param cfg Pointer to the cracking configuration structure.
 */
void enigma_crack_reflector_ioc(enigma_crack_config_t* cfg) {
    enigma_t enigma;
    memcpy(&enigma, cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        memcpy(&enigma.reflector, enigma_reflectors[i], sizeof(enigma_reflector_t));
        enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
        float score = enigma_ioc_score(plaintext, cfg);
        printf("Reflector %s: Score %.6f\n", enigma.reflector.name, score);
    }

    free(plaintext);
}

/**
 * @brief Crack the rotor positions using Index of Coincidence.
 *
 * This function attempts to determine the rotor starting positions of the Enigma machine
 * by evaluating all possible rotor positions and scoring the resulting plaintext
 * using the Index of Coincidence method.
 *
 * @param cfg Pointer to the cracking configuration structure.
 */
void enigma_crack_positions_ioc(enigma_crack_config_t* cfg) {
    enigma_t enigma;
    memcpy(&enigma, cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        enigma.rotors[0].idx = i;
        for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
            enigma.rotors[1].idx = j;
            for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                enigma.rotors[2].idx = k;
                enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
                float score = enigma_ioc_score(plaintext, cfg);
                printf("Positions %c%c%c: Score %.6f\n", 'A' + i, 'A' + j, 'A' + k, score);
            }
        }
    }

    free(plaintext);
}

/**
 * @brief Score text using Index of Coincidence.
 *
 * This function calculates the Index of Coincidence for the given text and returns
 * the absolute difference between the calculated score and the target score specified
 * in the provided enigma_crack_config_t structure.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 */
float enigma_ioc_score(const char* text, const enigma_crack_config_t* cfg) {
    int freq[26] = { 0 };
    float total = 0.0f;

    for (int i = 0; i < cfg->ciphertextLen; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }
        freq[text[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        total += (float)freq[i] * (freq[i] - 1);
    }

    float score = total / (float)(cfg->ciphertextLen * (cfg->ciphertextLen - 1));
    return fabs(score - cfg->targetScore);

}
