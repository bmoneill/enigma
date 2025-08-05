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
} enigma_rotor_t;

extern const enigma_rotor_t enigma_rotor_I;
extern const enigma_rotor_t enigma_rotor_II;
extern const enigma_rotor_t enigma_rotor_III;
extern const enigma_rotor_t enigma_rotor_IV;
extern const enigma_rotor_t enigma_rotor_V;
extern const enigma_rotor_t enigma_rotor_VI;
extern const enigma_rotor_t enigma_rotor_VII;
extern const enigma_rotor_t enigma_rotor_VIII;
extern const enigma_rotor_t* enigma_rotors[];

#endif
