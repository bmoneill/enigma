#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"
#include "ngram.h"

typedef struct {
    enigma_t        enigma;
    float           score;
} enigma_score_t;

typedef struct {
    enigma_t        enigma;
    int             method;
    int             target;
    char*           plaintext;
    int             plaintextPos;
    float           minScore;
    float           maxScore;
    int             n;
    enigma_ngram_t* ngrams;
    int             ngramCount;
    int             maxPlugboardSettings;
    int             threadCount;
    enigma_score_t* scores;
    int             scoreCount;
} enigma_crack_config_t;

#endif
