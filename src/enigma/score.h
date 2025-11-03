#ifndef LIBENIGMA_SCORE_H
#define LIBENIGMA_SCORE_H

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
 * @param scores      An array of enigma_score_t structures.
 * @param score_count The number of scores in the array.
 * @param max_scores  The maximum number of scores that can be stored in the array.
 */
typedef struct {
    enigma_score_t* scores;
    int             score_count;
    int             max_scores;
} enigma_score_list_t;

int enigma_score_print(const enigma_score_list_t*);
int enigma_score_list_sort(enigma_score_list_t*);

/* enigma_score_t getters and setters */
enigma_t* enigma_score_get_enigma(enigma_score_t*);
float     enigma_score_get_score(enigma_score_t*);
int       enigma_score_get_flags(enigma_score_t*);
int       enigma_score_set_enigma(enigma_score_t*, enigma_t*);
int       enigma_score_set_score(enigma_score_t*, float);
int       enigma_score_set_flags(enigma_score_t*, int);

/* enigma_score_list_t getters and setters */
enigma_score_t* enigma_score_list_get_scores(enigma_score_list_t*);
enigma_score_t* enigma_score_list_get_score(enigma_score_list_t*, int);
int             enigma_score_list_get_score_count(enigma_score_list_t*, int);
int             enigma_score_list_get_max_scores(enigma_score_list_t*);
int             enigma_score_list_set_score(enigma_score_list_t*, int, enigma_score_t*);
int             enigma_score_list_set_score_count(enigma_score_list_t*, int);
int             enigma_score_list_set_max_scores(enigma_score_list_t*, int);

#endif
