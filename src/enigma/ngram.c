/**
 * @file enigma/ngram.c
 *
 * This file implements cracking functions using n-gram scoring.
 */
#include "ngram.h"

#include "common.h"
#include "crack.h"
#include "io.h"
#include "threads.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Score text using bigram frequencies.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 * @param bigrams Array of bigram scores indexed by ENIGMA_BIIDX(a, b).
 *
 * @return The total bigram score.
 */
float enigma_bigram_score(const char* text, const enigma_crack_config_t* cfg) {
    float total = 0.0f;
    int next = text[0] - 'A';

    for (int i = 1; i < cfg->ciphertextLen; i++) {
        int cur = next;
        next = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next < 0 || next >= 26) {
            continue;
        }
        total += cfg->ngrams[ENIGMA_BIIDX(cur, next)];
    }

    return total / cfg->ciphertextLen;
}

/**
 * @brief Score text using trigram frequencies.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 * @param trigrams Array of trigram scores indexed by ENIGMA_TRIIDX(a, b, c).
 *
 * @return The total trigram score.
 */
float enigma_trigram_score(const char* text, const enigma_crack_config_t* cfg) {
    float total = 0.0f;
    int next1 = text[0] - 'A';
    int next2 = text[1] - 'A';

    for (int i = 2; i < cfg->ciphertextLen; i++) {
        int cur = next1;
        next1 = next2;
        next2 = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26) {
            continue;
        }
        total += cfg->ngrams[ENIGMA_TRIIDX(cur, next1, next2)];
    }

    return total / cfg->ciphertextLen;
}

/**
 * @brief Score text using quadgram frequencies.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 * @param quadgrams Array of quadgram scores indexed by ENIGMA_QUADIDX(a, b, c, d).
 *
 * @return The total quadgram score.
 */
float enigma_quadgram_score(const char* text, const enigma_crack_config_t* cfg) {
    float total = 0.0f;
    int next1 = text[0] - 'A';
    int next2 = text[1] - 'A';
    int next3 = text[2] - 'A';

    for (int i = 3; i < cfg->ciphertextLen; i++) {
        int cur = next1;
        next1 = next2;
        next2 = next3;
        next3 = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26 ||
            next3 < 0 || next3 >= 26) {
            continue;
        }
        total += cfg->ngrams[ENIGMA_QUADIDX(cur, next1, next2, next3)];
    }

    return total / cfg->ciphertextLen;
}
