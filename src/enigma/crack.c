/**
 * @file enigma/crack.c
 *
 * This file implements functions for cracking Enigma machine configurations
 * using various methods such as brute force and n-gram analysis. It includes
 * functions for loading dictionaries, scoring decrypted text, and finding potential
 * indices in the ciphertext where known plaintext may exist.
 */
#include "crack.h"

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include "enigma.h"
#include "rotors.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int score_compare(const void* a, const void* b);

pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Finds potential indices in the ciphertext where the known plaintext may exist.
 *
 * Due to the nature of the Enigma machine, a letter cannot be encoded to itself.
 * This function checks the ciphertext for potential indices where the given string
 * could potentially be in the plaintext.
 *
 * @param ciphertext The ciphertext to analyze
 * @param plaintext The known plaintext string to test against the ciphertext
 * @param indices Pointer to an array (length >= length of ciphertext) to store the indices (-1-terminated)
 */
void enigma_find_potential_indices(const char* ciphertext, const char* plaintext, int* indices) {
    int matchCount = 0;
    int plaintextLen = strlen(plaintext);
    int ciphertextLen = strlen(ciphertext);

    for (int i = 0; i < ciphertextLen - plaintextLen; i++) {
        for (int j = 0; j < plaintextLen; j++) {
            if (ciphertext[i + j] == plaintext[j]) {
                break;
            }
            if (j == plaintextLen - 1) {
                indices[matchCount++] = i;
            }
        }
    }

    indices[matchCount] = -1;
}

void enigma_load_dictionary(enigma_crack_config_t* cfg, FILE* dictFile) {
    char line[BUFSIZ];
    int index = 0;

    while (fgets(line, sizeof(line), dictFile) != NULL) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character
        cfg->dictionary[index] = strdup(line);
        index++;
    }
    cfg->dictSize = index;
}

void enigma_free_dictionary(enigma_crack_config_t* cfg) {
    for (int i = 0; i < cfg->dictSize; i++) {
        free((void*) cfg->dictionary[i]);
    }
    free(cfg->dictionary);
    cfg->dictionary = NULL;
    cfg->dictSize = 0;
}

/**
 * @brief Sort an array of enigma_score_t by score in descending order.
 *
 * @param scores The array of enigma_score_t to sort.
 * @param count The number of elements in the scores array.
 */
void enigma_score_sort(enigma_score_list_t *scoreList) {
    qsort(scoreList->scores, scoreList->scoreCount, sizeof(enigma_score_t), score_compare);
}

/**
 * @brief Print the scores in an enigma_score_list_t.
 *
 * @param scoreList Pointer to the enigma_score_list_t structure.
 */
void enigma_score_print(const enigma_score_list_t* scoreList) {
    for (int i = 0; i < scoreList->scoreCount; i++) {
        ENIGMA_PRINT_CONFIG(scoreList->scores[i].enigma);
        printf("%.6f\n", scoreList->scores[i].score);
    }
}

/**
 * @brief Append a score to an enigma_score_list_t.
 *
 * If the scores array is full, it will be resized to double its current size.
 *
 * @param scoreList Pointer to the enigma_score_list_t structure.
 * @param score The score to append.
 */
void enigma_score_append(enigma_score_list_t* scoreList, float score) {
    pthread_mutex_lock(&score_mutex);
    if (scoreList->scoreCount >= scoreList->maxScores) {
        scoreList->maxScores *= 2;
        scoreList->scores = realloc(scoreList->scores, scoreList->maxScores * sizeof(enigma_score_t));
    }

    scoreList->scores[scoreList->scoreCount].score = score;
    scoreList->scoreCount++;

    pthread_mutex_unlock(&score_mutex);
}

/**
 * @brief Compare function for sorting enigma_score_t by score.
 *
 * This function is used by qsort to sort an array of enigma_score_t structures
 * in descending order based on the score field.
 */
static int score_compare(const void *a, const void *b) {
    const enigma_score_t* scoreA = (const enigma_score_t*)a;
    const enigma_score_t* scoreB = (const enigma_score_t*)b;

    if (scoreA->score < scoreB->score) {
        return 1;
    } else if (scoreA->score > scoreB->score) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * @brief Calculate the frequency of characters in the plaintext.
 *
 * This function calculates the frequency of each character in the plaintext
 * and returns a score based on the frequencies.
 *
 * @param plaintext The plaintext to analyze
 * @return The frequency score of the plaintext
 */
float enigma_freq(const char* plaintext, int len) {
    int freq[26] = {0};
    for (int i = 0; i < len; i++) {
        if (plaintext[i] >= 'A' && plaintext[i] <= 'Z') {
            freq[plaintext[i] - 'A']++;
        }
    }

    float score = 0.0f;
    for (int i = 0; i < 26; i++) {
        score += (float)freq[i] * (freq[i] - 1);
    }
    return score / (len * (len - 1));
}

/**
 * @brief Check if the letter frequencies in the plaintext match target frequencies within an offset.
 *
 * This function checks if the frequency of each letter in the plaintext is within
 * the specified offset of the target frequencies. If more than half of the letters
 * deviate from the target frequencies, the function returns 0 (false). Otherwise,
 * it returns 1 (true).
 *
 * @param plaintext The plaintext to analyze
 * @param len The length of the plaintext
 * @param targets The target frequencies for each letter (array of 26 floats)
 * @param offset The allowable deviation from the target frequencies
 * @return 1 if over half of the letter frequencies match within the offset, 0 otherwise
 */
int enigma_letter_freq(const char* plaintext, int len,  float* targets, float offset, float* result) {
    int letters[26] = {0};
    int total = 0;
    for (int i = 0; i < len; i++) {
        letters[plaintext[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        float freq = (float)letters[i] / len * 100.0f;
        if (freq < targets[i] - offset || freq > targets[i] + offset) {
            total++;
            if (total > 13) {
                return 0;
            }
        }
    }
    *result = (float)total / 26.0f;
    return 1;
}

/**
 * @brief Checks if multiple words from the dictionary are present in the plaintext.
 *
 * This function checks the plaintext against a dictionary of words and returns 1
 * if multiple words are found, otherwise returns 0.
 *
 * The dictionary must be uppercase.
 *
 * @param plaintext The plaintext to check
 * @param cfg The enigma_crack_config_t containing the dictionary and its size
 *
 */
int enigma_dict_match(const char* plaintext, const enigma_crack_config_t* cfg) {
    int match_count = 0;
    for (int i = 0; i < cfg->dictSize; i++) {
        if (strstr(plaintext, cfg->dictionary[i]) != NULL) {
            match_count++;
            if (match_count > 1) {
                return 1;
            }
        }
    }
    return 0;
}
