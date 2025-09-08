#ifndef ENIGMA_IC_H
#define ENIGMA_IC_H

#include "enigma.h"
typedef struct {
    enigma_t* enigma;
    float score;
} crack_result_t;

static const float enigma_ic_english_min = 0.05769; // 1.5/26
static const float enigma_ic_english_max = 0.07692; // 2.0/26

crack_result_t *crack_rotors(enigma_t* enigma, const char* ciphertext, char* plaintext, int len, int count);

#endif
