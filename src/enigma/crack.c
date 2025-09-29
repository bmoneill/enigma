#include "crack.h"

#include "enigma.h"
#include "rotors.h"

#include <stdlib.h>
#include <string.h>

static int score_compare(const void *a, const void *b);

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
/**
 * @brief Sort an array of enigma_score_t by score in descending order.
 *
 * @param scores The array of enigma_score_t to sort.
 * @param count The number of elements in the scores array.
 */
void enigma_sort_scores(enigma_score_t *scores, int count) {
    qsort(scores, count, sizeof(enigma_score_t), score_compare);
}

/**
 * @brief Compare function for sorting enigma_score_t by score.
 *
 * This function is used by qsort to sort an array of enigma_score_t structures
 * in descending order based on the score field.
 */
static int score_compare(const void *a, const void *b) {
    const enigma_score_t *scoreA = (const enigma_score_t *)a;
    const enigma_score_t *scoreB = (const enigma_score_t *)b;

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
 * @brief Checks if multiple words from the dictionary are present in the plaintext.
 *
 * This function checks the plaintext against a dictionary of words and returns 1
 * if multiple words are found, otherwise returns 0.
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
