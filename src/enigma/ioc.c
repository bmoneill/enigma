/**
 * @file enigma/ioc.c
 *
 * This file implements cracking functions using the Index of Coincidence method.
 */
#include "ioc.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief Score text using Index of Coincidence.
 *
 * This function calculates the Index of Coincidence for the given text and returns
 *
 * @param text The text to score.
 * @param cfg Pointer to the cracking configuration structure.
 */
float enigma_ioc_score(const char* text, const enigma_crack_config_t* cfg) {
    int freq[26] = { 0 };
    float total = 0.0f;
    int len = strlen(text);

    for (int i = 0; i < len; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }
        freq[text[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        total += (float)freq[i] * (freq[i] - 1);
    }

    printf("Total: %.2f, Len: %d\n", total, len);
    float score = total / (float)(len * (len - 1));
    printf("Total: %.2f, Len: %d, Score: %.4f\n", total, len, score);
    return score;

}
