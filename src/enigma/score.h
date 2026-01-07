/**
 * @file enigma/score.h
 *
 * This file contains function definitions related to plaintext scoring.
 */
#ifndef ENIGMA_SCORE_H
#define ENIGMA_SCORE_H

#include "enigma.h"

/**
 * @brief Flag indicating that an analyzed configuration should, or does, have at least two dictionary word matches.
 */
#define ENIGMA_FLAG_DICTIONARY_MATCH 1

/**
 * @brief Flag indicating that an analyzed configuration should, or does, fall within a certain letter frequency range.
 */
#define ENIGMA_FLAG_FREQUENCY 2

/**
 * @brief Flag indicating that an analyzed configuration should, or does, contain known plaintext.
 */
#define ENIGMA_FLAG_KNOWN_PLAINTEXT 4

/**
 * @struct EnigmaScore
 * @brief A structure representing a scored Enigma configuration.
 *
 * @param enigma The Enigma configuration.
 * @param score  The score of the configuration.
 * @param flags  Flags indicating special conditions the configuration met (e.g. matches dictionary
 *               words or contains known plaintext).
 */
typedef struct {
    Enigma enigma;
    float  score;
    int    flags;
} EnigmaScore;

/**
 * @struct EnigmaScoreList
 * @brief A structure representing a list of scored Enigma configurations.
 *
 * @param scores      An array of EnigmaScore structs.
 * @param score_count The number of scores in the array.
 * @param max_scores  The maximum number of scores that can be stored in the array.
 */
typedef struct {
    EnigmaScore* scores;
    int          score_count;
    int          max_scores;
} EnigmaScoreList;

int enigma_score_print(const EnigmaScoreList*);
int enigma_score_list_sort(EnigmaScoreList*);

/* --- EnigmaScore getters and setters --- */
Enigma* enigma_score_get_enigma(EnigmaScore*);
float   enigma_score_get_score(EnigmaScore*);
int     enigma_score_get_flags(EnigmaScore*);
int     enigma_score_set_enigma(EnigmaScore*, Enigma*);
int     enigma_score_set_score(EnigmaScore*, float);
int     enigma_score_set_flags(EnigmaScore*, int);

/* --- EnigmaScoreList getters and setters --- */
EnigmaScore* enigma_score_list_get_scores(EnigmaScoreList*);
EnigmaScore* enigma_score_list_get_score(EnigmaScoreList*, int);
int          enigma_score_list_get_score_count(EnigmaScoreList*);
int          enigma_score_list_get_max_scores(EnigmaScoreList*);
int          enigma_score_list_set_score(EnigmaScoreList*, int, EnigmaScore*);
int          enigma_score_list_set_score_count(EnigmaScoreList*, int);
int          enigma_score_list_set_max_scores(EnigmaScoreList*, int);

#endif
