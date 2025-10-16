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

#define BIIDX(a, b) ((a << 5) | b)
#define TRIIDX(a, b, c) ((a << 10) | (b << 5) | c)
#define QUADIDX(a, b, c, d) ((a << 15) | (b << 10) | (c << 5) | d)

/**
 * @brief Score text using bigram frequencies.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 * @param bigrams Array of bigram scores indexed by BIIDX(a, b).
 *
 * @return The absolute difference between the total bigram score and the target score.
 */
float enigma_bigram_score(const char* text, const enigma_crack_config_t* cfg, const float* bigrams) {
    float total = 0.0f;
    int next = text[0] - 'A';

    for (int i = 1; i < cfg->ciphertextLen; i++) {
        int cur = next;
        next = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next < 0 || next >= 26) {
            continue;
        }
        total += bigrams[BIIDX(cur, next)];
    }

    return fabs(total - cfg->targetScore);
}

/**
 * @brief Score text using trigram frequencies.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 * @param trigrams Array of trigram scores indexed by TRIIDX(a, b, c).
 *
 * @return The absolute difference between the total trigram score and the target score.
 */
float enigma_trigram_score(const char* text, const enigma_crack_config_t* cfg, const float* trigrams) {
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
        total += trigrams[TRIIDX(cur, next1, next2)];
    }

    return fabs(total - cfg->targetScore);
}

/**
 * @brief Score text using quadgram frequencies.
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 * @param quadgrams Array of quadgram scores indexed by QUADIDX(a, b, c, d).
 *
 * @return The absolute difference between the total quadgram score and the target score.
 */
float enigma_quadram_score(const char* text, const enigma_crack_config_t* cfg, const float* quadgrams) {
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
        total += quadgrams[QUADIDX(cur, next1, next2, next3)];
    }

    return fabs(total - cfg->targetScore);
}

/**
 * @brief Analyze decrypted text using n-gram scoring.
 *
 * This function scores the decrypted text using the n-gram method.
 *
 * @param plaintext The decrypted text to analyze.
 * @param cfg Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_ngram_analyze(const char* plaintext, const enigma_crack_config_t* cfg, const enigma_ngram_list_t* ngramList) {
    float score = 0.0f;
    int textLen = cfg->ciphertextLen;

    switch (ngramList->n) {
    case 2: score = enigma_bigram_score(plaintext, cfg, (const float*)ngramList->ngrams); break;
    case 3: score = enigma_trigram_score(plaintext, cfg, (const float*)ngramList->ngrams); break;
    case 4: score = enigma_quadram_score(plaintext, cfg, (const float*)ngramList->ngrams); break;
    }

    enigma_score_append(cfg->scores, score);
}
