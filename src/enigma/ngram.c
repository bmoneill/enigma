/**
 * @file enigma/ngram.c
 *
 * This file implements cracking functions using n-gram scoring.
 */
#include "ngram.h"

#include "common.h"
#include "crack.h"

/**
 * @brief Score text using bigram frequencies.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param text The text to score.
 *
 * @return The total bigram score.
 */
EMSCRIPTEN_KEEPALIVE float enigma_bigram_score(const EnigmaCrackParams* cfg, const char* text) {
    float total = 0.0f;
    int   next  = text[0] - 'A';

    for (size_t i = 1; i < cfg->ciphertext_length; i++) {
        int cur = next;
        next    = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next < 0 || next >= 26) {
            continue;
        }
        total += cfg->ngrams[ENIGMA_BIIDX(cur, next)];
    }

    return total / cfg->ciphertext_length;
}

/**
 * @brief Score text using trigram frequencies.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param text The text to score.
 *
 * @return The total trigram score.
 */
EMSCRIPTEN_KEEPALIVE float enigma_trigram_score(const EnigmaCrackParams* cfg, const char* text) {
    float total = 0.0f;
    int   next1 = text[0] - 'A';
    int   next2 = text[1] - 'A';

    for (size_t i = 2; i < cfg->ciphertext_length; i++) {
        int cur = next1;
        next1   = next2;
        next2   = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26) {
            continue;
        }
        total += cfg->ngrams[ENIGMA_TRIIDX(cur, next1, next2)];
    }

    return total / cfg->ciphertext_length;
}

/**
 * @brief Score text using quadgram frequencies.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param text The text to score.
 *
 * @return The total quadgram score.
 */
EMSCRIPTEN_KEEPALIVE float enigma_quadgram_score(const EnigmaCrackParams* cfg, const char* text) {
    float total = 0.0f;
    int   next1 = text[0] - 'A';
    int   next2 = text[1] - 'A';
    int   next3 = text[2] - 'A';

    for (size_t i = 3; i < cfg->ciphertext_length; i++) {
        int cur = next1;
        next1   = next2;
        next2   = next3;
        next3   = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26
            || next3 < 0 || next3 >= 26) {
            continue;
        }
        total += cfg->ngrams[ENIGMA_QUADIDX(cur, next1, next2, next3)];
    }

    return total / cfg->ciphertext_length;
}
