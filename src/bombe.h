#ifndef ENIGMA_BOMBE_H
#define ENIGMA_BOMBE_H

#include "enigma.h"

typedef struct {
    char *crib;
    int cribLength;
    int cribIndex;
    int numCribs;
} bombe_t;

void enigma_bombe_init(bombe_t*, char*, int);
void enigma_bombe_run(bombe_t*, const char*, int);
void enigma_bombe_find_potential_indices(const char*, const char*, int*);

#endif
