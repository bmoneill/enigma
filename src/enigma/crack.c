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
#include "io.h"
#include "rotors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int score_compare(const void* a, const void* b);

/**
 * @brief Crack the plugboard using a scoring function.
 *
 * This function attempts to determine the plugboard settings used in the Enigma machine
 * by evaluating all possible plugboard configurations and scoring the resulting plaintext
 * using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 */
void enigma_crack_plugboard(enigma_crack_config_t* cfg, float (*scoreFunc)(const char*, const enigma_crack_config_t*)) {
    enigma_t enigma;
    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    int curSettings = strlen(enigma.plugboard) / 2;

    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));
    char* remaining = malloc((ENIGMA_ALPHA_SIZE - curSettings * 2 + 1) * sizeof(char));
    memcpy(remaining, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ENIGMA_ALPHA_SIZE);

    for (int i = 0; i < curSettings * 2; i++) {
        remaining[enigma.plugboard[i] - 'A'] = '\0';
    }

    for (char a = 'A'; a < 'Z'; a++) {
        if (!remaining[a - 'A']) {
            continue;
        }
        for (char b = 'A'; b <= 'Z'; b++) {
            if (a == b || !remaining[b - 'A']) {
                continue;
            }

            enigma.plugboard[curSettings * 2] = a;
            enigma.plugboard[curSettings * 2 + 1] = b;
            enigma.plugboard[curSettings * 2 + 2] = '\0';

            enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
            enigma_score_append(cfg, &enigma, plaintext, scoreFunc(plaintext, cfg));
        }
    }

    free(plaintext);
    free(remaining);
}

/**
 * @brief Crack the reflector using a scoring function.
 *
 * This function attempts to determine the reflector used in the Enigma machine
 * by evaluating all possible reflectors and scoring the resulting plaintext
 * using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 */
void enigma_crack_reflector(enigma_crack_config_t* cfg, float (*scoreFunc)(const char*, const enigma_crack_config_t*)) {
    enigma_t enigma;
    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        enigma.reflector = enigma_reflectors[i];
        enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(plaintext, cfg));
    }

    free(plaintext);
}

/**
 * @brief Crack the rotor using a scoring function.
 *
 * This function attempts to determine the rotor used in the Enigma machine
 * by evaluating all possible rotors and scoring the resulting plaintext
 * using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param targetRotor The index of the rotor to crack (0-based).
 * @param scoreFunc Function pointer to the scoring function to use.
 */
void enigma_crack_rotor(enigma_crack_config_t* cfg, int targetRotor, float (*scoreFunc)(const char*, const enigma_crack_config_t*)) {
    enigma_t enigma;
    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        enigma.rotors[targetRotor] = enigma_rotors[i];
        enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(plaintext, cfg));
    }

    free(plaintext);
}

/**
 * @brief Crack the rotor order using a scoring function.
 *
 * This function attempts to determine the order of rotors used in the Enigma machine
 * by evaluating all possible rotor orders and scoring the resulting plaintext
 * using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 */
void enigma_crack_rotors(enigma_crack_config_t* cfg, float (*scoreFunc)(const char*, const enigma_crack_config_t*)) {
    enigma_t enigma;
    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
            if (i == j) continue;
            for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                if (j == k || i == k) continue;
                enigma.rotors[0] = enigma_rotors[i];
                enigma.rotors[1] = enigma_rotors[j];
                enigma.rotors[2] = enigma_rotors[k];

                enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
                enigma_score_append(cfg, &enigma, plaintext, scoreFunc(plaintext, cfg));
            }
        }
    }

    free(plaintext);
}

/**
 * @brief Crack the rotor positions using a scoring function.
 *
 * This function attempts to determine the rotor starting positions of the Enigma machine
 * by evaluating all possible rotor positions and scoring the resulting plaintext
 * using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 */
void enigma_crack_rotor_positions(enigma_crack_config_t* cfg, float (*scoreFunc)(const char*, const enigma_crack_config_t*)) {
    enigma_t enigma;
    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    char* plaintext = malloc((cfg->ciphertextLen + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
            for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                enigma.rotor_indices[0] = i;
                enigma.rotor_indices[1] = j;
                enigma.rotor_indices[2] = k;

                enigma_encode_string(&enigma, cfg->ciphertext, plaintext, cfg->ciphertextLen);
                enigma_score_append(cfg, &enigma, plaintext, scoreFunc(plaintext, cfg));
            }
        }
    }

    free(plaintext);
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
 * @brief Calculate the frequency of characters in the plaintext.
 *
 * This function calculates the frequency of each character in the plaintext
 * and returns a score based on the frequencies.
 *
 * @param plaintext The plaintext to analyze
 * @param len The length of the plaintext
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
 * the specified offset of the target frequencies. If more than ten letters
 * deviate from the target frequencies, the function returns 0 (false). Otherwise,
 * it returns 1 (true).
 *
 * @param plaintext The plaintext to analyze
 * @param cfg       The enigma_crack_config_t containing the target frequencies and offset
 * @return 1 if over half of the letter frequencies match within the offset, 0 otherwise
 */
int enigma_letter_freq(const char* plaintext, const enigma_crack_config_t* cfg) {
    int letters[26] = {0};
    int total = 0;
    for (int i = 0; i < cfg->ciphertextLen; i++) {
        letters[plaintext[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        float freq = (float)letters[i] / cfg->ciphertextLen * 100.0f;
        if (freq < cfg->freqTargets[i] - cfg->minFreq || freq > cfg->freqTargets[i] + cfg->maxFreq) {
            total++;
            if (total > 10) {
                return 0;
            }
        }
    }
    return 1;
}

/**
 * @brief Append a score to an enigma_score_list_t.
 *
 * If the scores array is full, it will be resized to double its current size.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param enigma Pointer to the enigma_t structure representing the scored configuration.
 * @param plaintext The plaintext corresponding to the score.
 * @param score The score to append.
 */
void enigma_score_append(enigma_crack_config_t* cfg, enigma_t* enigma, const char* plaintext, float score) {
    if (cfg->scores->scoreCount >= cfg->scores->maxScores) {
        cfg->scores->maxScores *= 2;
        cfg->scores->scores = realloc(cfg->scores->scores, cfg->scores->maxScores * sizeof(enigma_score_t));
    }

    memcpy(&cfg->scores->scores[cfg->scores->scoreCount].enigma, enigma, sizeof(enigma_t));
    cfg->scores->scores[cfg->scores->scoreCount].score = score;
    cfg->scores->scores[cfg->scores->scoreCount].flags = enigma_score_flags(plaintext, cfg);
    cfg->scores->scoreCount++;
}

/**
 * @brief Get the flags for a given plaintext based on the crack configuration.
 *
 * This function checks the plaintext against the criteria specified in the
 * enigma_crack_config_t and returns a bitmask of flags indicating which criteria
 * were met.
 *
 * @param plaintext The plaintext to evaluate.
 * @param cfg The enigma_crack_config_t containing the criteria and flags.
 * @return A bitmask of flags indicating which criteria were met.
 */
int enigma_score_flags(const char* plaintext, const enigma_crack_config_t* cfg) {
    int ret = 0;
    if (cfg->flags & ENIGMA_FLAG_DICTIONARY_MATCH && enigma_dict_match(plaintext, cfg)) {
        ret |= ENIGMA_FLAG_DICTIONARY_MATCH;
    }

    if (cfg->flags & ENIGMA_FLAG_FREQUENCY) {
        float freqScore;
        if (enigma_letter_freq(plaintext, cfg)) {
            ret |= ENIGMA_FLAG_FREQUENCY;
        }
    }

    if (cfg->flags & ENIGMA_FLAG_KNOWN_PLAINTEXT && strstr(plaintext, cfg->ciphertext)) {
        ret |= ENIGMA_FLAG_KNOWN_PLAINTEXT;
    }
    return ret;
}
/**
 * @brief Print the scores in an enigma_score_list_t.
 *
 * @param scoreList Pointer to the enigma_score_list_t structure.
 */
void enigma_score_print(const enigma_score_list_t* scoreList) {
    char buf[64];
    for (int i = 0; i < scoreList->scoreCount; i++) {
        printf("%.6f ", scoreList->scores[i].score);
        if (scoreList->scores[i].flags & ENIGMA_FLAG_DICTIONARY_MATCH) {
            printf("D");
        } else {
            printf("-");
        }
        if (scoreList->scores[i].flags & ENIGMA_FLAG_FREQUENCY) {
            printf("F ");
        } else {
            printf("- ");
        }
        enigma_print_config(&scoreList->scores[i].enigma, buf);
        printf("%s\n", buf);
    }
}

/**
 * @brief Sort an array of enigma_score_t by score in descending order.
 *
 * @param scores The array of enigma_score_t to sort.
 */
void enigma_score_sort(enigma_score_list_t *scoreList) {
    qsort(scoreList->scores, scoreList->scoreCount, sizeof(enigma_score_t), score_compare);
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

