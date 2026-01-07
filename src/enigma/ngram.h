/**
 * @file enigma/ngram.h
 *
 * This file declares functions for cracking Enigma configurations
 * using n-gram scoring.
 */
#ifndef ENIGMA_NGRAM_H
#define ENIGMA_NGRAM_H

#include "crack.h"

/**
 * @brief Generate a bigram index from two characters (for array lookup)
 */
#define ENIGMA_BIIDX(a, b) ((a << 5) | b)

/**
 * @brief Generate a trigram index from three characters (for array lookup)
 */
#define ENIGMA_TRIIDX(a, b, c) ((a << 10) | (b << 5) | c)

/**
 * @brief Generate a quadgram index from four characters (for array lookup)
 */
#define ENIGMA_QUADIDX(a, b, c, d) ((a << 15) | (b << 10) | (c << 5) | d)

float enigma_bigram_score(const EnigmaCrackParams*, const char*);
float enigma_trigram_score(const EnigmaCrackParams*, const char*);
float enigma_quadgram_score(const EnigmaCrackParams*, const char*);

#endif
