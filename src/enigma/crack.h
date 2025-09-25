#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"

#define ENIGMA_PREDEFINED_PLUGBOARD_SETTINGS 1
#define ENIGMA_PREDEFINED_ROTOR_SETTINGS 2
#define ENIGMA_PREDEFINED_ROTOR_POSITIONS 4
#define ENIGMA_PREDEFINED_REFLECTOR 8

typedef struct {
    enigma_t        enigma;
    float           score;
} enigma_score_t;

typedef struct {
    enigma_t             enigma;
    int                  method;
    int                  target;
    const char*          plaintext;
    int                  plaintextPos;
    int                  plaintextLen;
    const char*          ciphertext;
    int                  ciphertextLen;
    float                minScore;
    float                maxScore;
    int                  ngramCount;
    int                  maxPlugboardSettings;
    int                  maxThreads;
    enigma_score_t*      scores;
    const char*          dictionary;
    int                  scoreCount;
    int                  flags;
} enigma_crack_config_t;

void enigma_find_potential_indices(const char*, const char*, int*);
float enigma_freq(const char*, int);

#endif
