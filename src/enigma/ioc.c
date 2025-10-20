/**
 * @file enigma/ioc.c
 *
 * This file implements cracking functions using the Index of Coincidence method.
 */
#include "ioc.h"

/**
 * @brief Score text using Index of Coincidence.
 *
 * This function calculates the Index of Coincidence for the given text and returns
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param text The text to score.
 */
float enigma_ioc_score(const enigma_crack_config_t* cfg, const char* text) {
    int freq[26] = { 0 };
    float total = 0.0f;
    int len = cfg->ciphertextLen;

    for (int i = 0; i < len; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }
        freq[text[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        total += (float)freq[i] * (freq[i] - 1);
    }

    float score = total / (float)(len * (len - 1));
    return score;

}
