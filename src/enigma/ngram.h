/**
 * @file enigma/ngram.h
 *
 * This file declares functions for cracking Enigma configurations
 * using n-gram scoring.
 */
#ifndef ENIGMA_NGRAM_H
#define ENIGMA_NGRAM_H

#include "crack.h"

float enigma_bigram_score (const char*,                  const enigma_crack_config_t*, const float*);
float enigma_trigram_score(const char*,                  const enigma_crack_config_t*, const float*);
float enigma_quadram_score(const char*,                  const enigma_crack_config_t*, const float*);
void  enigma_ngram_analyze(const char*,                  const enigma_crack_config_t*, const enigma_ngram_list_t*);

#endif
