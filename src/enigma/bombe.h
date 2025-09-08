#ifndef ENIGMA_BOMBE_H
#define ENIGMA_BOMBE_H

#include "crack.h"

typedef struct {
    char *crib;
    int cribLength;
    int cribIndex;
    int numCribs;
} enigma_bombe_t;

void enigma_crack_bombe(enigma_crack_config_t*);

void enigma_bombe_init(enigma_bombe_t*, char*, int);
void enigma_bombe_run(const enigma_bombe_t*, const char*, int);
void enigma_bombe_find_potential_indices(const char*, const char*, int*);

#endif
