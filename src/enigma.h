#ifndef ENIGMA_H
#define ENIGMA_H

#include "reflectors.h"
#include "rotors.h"

#define ROTOR_COUNT 8
#define REFLECTOR_COUNT 3

typedef struct {
    rotor_t rotors[4];
    int rotor_flag;
    int rotor_count;
    reflector_t* reflector;
    const char* plugboard;
} enigma_t;

char encode(enigma_t*, char);
void deinit_enigma(enigma_t*);
void init_rotors(enigma_t*, const rotor_t*, int);
void init_default_enigma(enigma_t*);

#endif
