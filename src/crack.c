#include "crack.h"

#include "enigma.h"
#include "rotors.h"

#include <stdlib.h>
#include <string.h>

#define NUMROTORCONFIGURATIONS (ROTOR_COUNT * (ROTOR_COUNT - 1) * (ROTOR_COUNT - 2))

static int score_compare(const void *a, const void *b);

/**
 * @brief Calculate the score for a given rotor configuration.
 * This function evaluates each rotor configuration by encoding the ciphertext
 * and scoring the resulting plaintext using the provided scoring function.
 * It returns an array of enigma_score_t structures containing the scores
 * and configurations in descending order.
 *
 * The scoring function must take the "plaintext" string (as const char *),
 * the length of the plaintext, and a pointer to additional data (if needed),
 * or NULL if not.
 *
 * @param ciphertext The ciphertext to decode.
 * @param len The length of the ciphertext.
 * @param scoreFunction The function to score the plaintext.
 * @param data Additional data to pass to the scoring function.
 */
enigma_score_t *enigma_rotor_configuration_score(const char *ciphertext, int len, const void *scoreFunction, float *data) {
    enigma_score_t *result = malloc(sizeof(enigma_score_t) * NUMROTORCONFIGURATIONS);
    char *plaintext = malloc(len + 1);
    enigma_score_t *cur = &result[0];
    enigma_init_default_config(&cur->enigma);

    for (int i = 0; i < ROTOR_COUNT; i++) {
        memcpy(&cur->enigma.rotors[0], enigma_rotors[i], sizeof(rotor_t));
        for (int j = 0; j < ROTOR_COUNT; j++) {
            if (i == j) {
                continue;
            }
            memcpy(&cur->enigma.rotors[1], enigma_rotors[j], sizeof(rotor_t));
            for (int k = 0; k < ROTOR_COUNT; k++) {
                if (k == i || k == j) {
                    continue;
                }
                memcpy(&cur->enigma.rotors[2], enigma_rotors[k], sizeof(rotor_t));

                enigma_encode_string(&cur->enigma, ciphertext, plaintext, len);
                cur->score = ((float (*)(const char *, int, float *))scoreFunction)(plaintext, len, data);
                cur->level = ENIGMA_ROTOR_POSITIONS;
                cur++;
                enigma_init_default_config(&cur->enigma);
            }
        }
    }

    qsort(result, NUMROTORCONFIGURATIONS, sizeof(enigma_score_t), score_compare);

    free(plaintext);
    return result;
}

/**
 * @brief Compare function for sorting enigma_score_t by score.
 *
 * This function is used by qsort to sort an array of enigma_score_t structures
 * in descending order based on the score field.
 */
static int score_compare(const void *a, const void *b) {
    const enigma_score_t *scoreA = (const enigma_score_t *)a;
    const enigma_score_t *scoreB = (const enigma_score_t *)b;

    if (scoreA->score < scoreB->score) {
        return 1;
    } else if (scoreA->score > scoreB->score) {
        return -1;
    } else {
        return 0;
    }
}
