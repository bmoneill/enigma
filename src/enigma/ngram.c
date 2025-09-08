#include "ngram.h"

#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#define BIIDX(a, b) ((a << 5) | b)
#define TRIIDX(a, b, c) ((a << 10) | (b << 5) | c)
#define QUADIDX(a, b, c, d) ((a << 15) | (b << 10) | (c << 5) | d)

/**
 * @brief Crack rotor configuration using n-gram scoring.
 *
 * Currently unimplemented.
 *
 * @param config Pointer to the cracking configuration structure.
 */
void enigma_crack_rotors_ngram(enigma_crack_config_t* config) {
    // TODO Implement
}

/**
 * @brief Crack rotor positions using n-gram scoring.
 *
 * Currently unimplemented.
 *
 * @param config Pointer to the cracking configuration structure.
 */
void enigma_crack_rotor_positions_ngram(enigma_crack_config_t* config) {
    // TODO Implement
}

/**
 * @brief Crack reflector using n-gram scoring.
 *
 * Currently unimplemented.
 *
 * @param config Pointer to the cracking configuration structure.
 * @todo Implement
 */
void enigma_crack_reflector_ngram(enigma_crack_config_t* config) {
    // TODO Implement
}

/**
 * @brief Crack plugboard settings using n-gram scoring.
 *
 * Currently unimplemented.
 *
 * @param config Pointer to the cracking configuration structure.
 * @todo Implement
 */
void enigma_crack_plugboard_ngram(enigma_crack_config_t* config) {
    // TODO Implement
}

/**
 * @brief Score text using bigram frequencies.
 *
 * @param text The text to score.
 * @param textLen The length of the text.
 * @param bigrams Array of bigram scores indexed by BIIDX(a, b).
 *
 * @return The total bigram score for the text.
 */
float enigma_bigram_score(const char* text, int textLen, const float* bigrams) {
    float total = 0.0f;
    int next = text[0] - 'A';

    for (int i = 1; i < textLen; i++) {
        int cur = next;
        next = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next < 0 || next >= 26) {
            continue;
        }
        total += bigrams[BIIDX(cur, next)];
    }

    return total;
}

/**
 * @brief Score text using trigram frequencies.
 *
 * @param text The text to score.
 * @param textLen The length of the text.
 * @param trigrams Array of trigram scores indexed by TRIIDX(a, b, c).
 *
 * @return The total trigram score for the text.
 */
float enigma_trigram_score(const char* text, int textLen, const float* trigrams) {
    float total = 0.0f;
    int next1 = text[0] - 'A';
    int next2 = text[1] - 'A';

    for (int i = 2; i < textLen; i++) {
        int cur = next1;
        next1 = next2;
        next2 = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26) {
            continue;
        }
        total += trigrams[TRIIDX(cur, next1, next2)];
    }

    return total;
}

/**
 * @brief Score text using quadgram frequencies.
 *
 * @param text The text to score.
 * @param textLen The length of the text.
 * @param quadgrams Array of quadgram scores indexed by QUADIDX(a, b, c, d).
 *
 * @return The total quadgram score for the text.
 */
float enigma_quadram_score(const char* text, int textLen, const float* quadgrams) {
    float total = 0.0f;
    int next1 = text[0] - 'A';
    int next2 = text[1] - 'A';
    int next3 = text[2] - 'A';

    for (int i = 3; i < textLen; i++) {
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

    return total;
}
