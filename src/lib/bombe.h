#ifndef ENIGMA_BOMBE_H
#define ENIGMA_BOMBE_H

#include "enigma.h"

typedef struct {
    char *crib;
    int cribLength;
    int cribIndex;
    int numCribs;
} bombe_t;

void bombe_init(bombe_t*, char*, int);
void bombe_run(bombe_t*, const char*, int);
void bombe_find_potential_indices(const char*, const char*, int*);

#endif
