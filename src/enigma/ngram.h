/**
 * @file enigma/ngram.h
 *
 * This file declares functions for cracking Enigma configurations
 * using n-gram scoring.
 */
#ifndef ENIGMA_NGRAM_H
#define ENIGMA_NGRAM_H

#include "crack.h"

#define ENIGMA_BIIDX(a, b) ((a << 5) | b)
#define ENIGMA_TRIIDX(a, b, c) ((a << 10) | (b << 5) | c)
#define ENIGMA_QUADIDX(a, b, c, d) ((a << 15) | (b << 10) | (c << 5) | d)

float enigma_bigram_score (const char*,                  const enigma_crack_config_t*);
float enigma_trigram_score(const char*,                  const enigma_crack_config_t*);
float enigma_quadram_score(const char*,                  const enigma_crack_config_t*);

#endif
