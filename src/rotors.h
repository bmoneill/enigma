#ifndef ENIGMA_ROTORS_H
#define ENIGMA_ROTORS_H

#include "defs.h"

typedef struct {
    const char* name;
    int fwd_indices[ALPHA_SIZE];
    int rev_indices[ALPHA_SIZE];
    int notches[2];
    int numNotches;
    int idx;
} rotor_t;

extern const rotor_t rotor_I;
extern const rotor_t rotor_II;
extern const rotor_t rotor_III;
extern const rotor_t rotor_IV;
extern const rotor_t rotor_V;
extern const rotor_t rotor_VI;
extern const rotor_t rotor_VII;
extern const rotor_t rotor_VIII;
extern const rotor_t* enigma_rotors[];

#endif
