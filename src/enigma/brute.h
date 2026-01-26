/**
 * @file enigma/brute.c
 *
 * This file implements scoring functions for brute force dictionary cracking
 * of Enigma machines that were hopefully already partially cracked.
 */
#ifndef ENIGMA_BRUTE_H
#define ENIGMA_BRUTE_H

#include "crack.h"

float enigma_brute_score(const EnigmaCrackParams*, const char*);

#endif
