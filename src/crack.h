#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"

typedef enum {
    ENIGMA_ROTOR_POSITIONS,
    ENIGMA_ROTOR_ALPHABET_POSITIONS,
    ENIGMA_ROTOR_PLUGBOARD
} enigma_cracklevel_t;

typedef struct {
    enigma_t enigma;
    float score;
    enigma_cracklevel_t level;
} enigma_score_t;

#endif
