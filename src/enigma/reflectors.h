#ifndef ENIGMA_REFLECTORS_H
#define ENIGMA_REFLECTORS_H

#include "defs.h"

typedef struct {
    const char* name;
    int indices[ALPHA_SIZE];
} enigma_reflector_t;

extern enigma_reflector_t enigma_UKW_A;
extern enigma_reflector_t enigma_UKW_B;
extern enigma_reflector_t enigma_UKW_C;
extern enigma_reflector_t* enigma_reflectors[];

#endif
