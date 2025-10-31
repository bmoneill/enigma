#ifndef ENIGMA_SCORE_H
#define ENIGMA_SCORE_H

#include "enigma.h"

#define ENIGMA_FLAG_DICTIONARY_MATCH 1
#define ENIGMA_FLAG_FREQUENCY        2
#define ENIGMA_FLAG_KNOWN_PLAINTEXT  4

/**
 * @struct enigma_score_t
 * @brief A structure representing a scored Enigma configuration.
 *
 * @param enigma The Enigma configuration.
 * @param score  The score of the configuration.
 * @param flags  Flags indicating special conditions the configuration met (e.g. matches dictionary
 *               words or contains known plaintext).
 */
typedef struct {
    enigma_t enigma;
    float    score;
    int      flags;
} enigma_score_t;

/**
 * @struct enigma_score_list_t
 * @brief A structure representing a list of scored Enigma configurations.
 *
 * @param scores     An array of enigma_score_t structures.
 * @param scoreCount The number of scores in the array.
 * @param maxScores  The maximum number of scores that can be stored in the array.
 */
typedef struct {
    enigma_score_t* scores;
    int             scoreCount;
    int             maxScores;
} enigma_score_list_t;

int   enigma_score_print(const enigma_score_list_t*);
int   enigma_score_sort(enigma_score_list_t*);

#endif
