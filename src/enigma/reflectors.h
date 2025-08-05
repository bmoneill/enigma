#ifndef ENIGMA_REFLECTORS_H
#define ENIGMA_REFLECTORS_H

#include "defs.h"

typedef struct {
    const char* name;
    int indices[ALPHA_SIZE];
} reflector_t;

extern reflector_t enigma_UKW_A;
extern reflector_t enigma_UKW_B;
extern reflector_t enigma_UKW_C;
extern reflector_t* enigma_reflectors[];

#endif
