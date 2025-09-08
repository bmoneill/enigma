#ifndef ENIGMA_H
#define ENIGMA_H

#include "reflectors.h"
#include "rotors.h"

#define ROTOR_COUNT 8
#define REFLECTOR_COUNT 3

typedef struct {
    enigma_rotor_t rotors[4];
    int rotor_flag;
    int rotor_count;
    reflector_t    reflector;
    const char* plugboard;
} enigma_t;

char enigma_encode(enigma_t*, char);
void enigma_encode_string(enigma_t*, const char*, char*, int);
void enigma_init_rotors(enigma_t*, const enigma_rotor_t*, int);
void enigma_init_default_config(enigma_t*);

#endif
