#ifndef ENIGMA_BOMBE_H
#define ENIGMA_BOMBE_H

#include "enigma.h"

#define MAX_CRIB_STRINGS 10
#define MAX_CRIB_INDICES 50

typedef struct {
    char* s;
    int length;
    int index;
} bombe_crib_node_t;

typedef struct {
    bombe_crib_node_t crib[MAX_CRIB_STRINGS];
    int lastCribIndex;
    int numCribs;
} bombe_t;

void bombe_init(bombe_t*, char**, int*, int);
void bombe_run(bombe_t*, const char*, int);
void bombe_find_potential_indices(const char*, const char*, int*);

#endif
