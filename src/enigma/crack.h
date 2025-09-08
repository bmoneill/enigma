#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"

typedef struct {
    enigma_t        enigma;
    float           score;
} enigma_score_t;

typedef struct {
    enigma_t             enigma;
    int                  method;
    int                  target;
    char*                plaintext;
    int                  plaintextPos;
    char*                ciphertext;
    int                  ciphertextLen;
    float                minScore;
    float                maxScore;
    int                  ngramCount;
    int                  maxPlugboardSettings;
    int                  maxThreads;
    enigma_score_t*      scores;
    int                  scoreCount;
} enigma_crack_config_t;

#endif
