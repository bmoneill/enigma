/**
 * @file enigma/crack.c
 *
 * This file implements functions for cracking Enigma machine configurations
 * using various methods such as brute force and n-gram analysis. It includes
 * functions for loading dictionaries, scoring decrypted text, and finding
 * potential indices in the ciphertext where known plaintext may exist.
 */
#include "crack.h"

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include "common.h"
#include "enigma.h"
#include "rotor.h"

#include <stdlib.h>
#include <string.h>

static int score_compare(const void* a, const void* b);

/**
 * @brief Crack the plugboard using a scoring function.
 *
 * This function attempts to determine the plugboard settings used in the Enigma
 * machine by evaluating all possible plugboard configurations and scoring the
 * resulting plaintext using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 *
 * @return 0 on success, non-zero on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_plugboard(enigma_crack_t* cfg,
                                                float (*scoreFunc)(const enigma_crack_t*,
                                                                   const char*)) {
    if (!cfg || !scoreFunc) {
        return 1;
    }

    enigma_t enigma;
    enigma_t enigmaTmp;
    int      curSettings = strlen(cfg->enigma.plugboard) / 2;

    char*    plaintext   = malloc((cfg->ciphertext_length + 1) * sizeof(char));
    char*    remaining   = malloc((ENIGMA_ALPHA_SIZE - curSettings * 2 + 1) * sizeof(char));
    memcpy(remaining, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ENIGMA_ALPHA_SIZE);

    for (int i = 0; i < curSettings * 2; i++) {
        remaining[enigma.plugboard[i] - 'A'] = '\0';
    }

    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    for (char a = 'A'; a < 'Z'; a++) {
        if (!remaining[a - 'A']) {
            continue;
        }
        for (char b = 'A'; b <= 'Z'; b++) {
            if (a == b || !remaining[b - 'A']) {
                continue;
            }

            enigma.plugboard[curSettings * 2]     = a;
            enigma.plugboard[curSettings * 2 + 1] = b;
            enigma.plugboard[curSettings * 2 + 2] = '\0';
            memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));

            enigma_encode_string(&enigmaTmp, cfg->ciphertext, plaintext, cfg->ciphertext_length);
            enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
        }
    }

    free(plaintext);
    free(remaining);

    return 0;
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
 *
 * @return 0 on success, non-zero on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_reflector(enigma_crack_t* cfg,
                                                float (*scoreFunc)(const enigma_crack_t*,
                                                                   const char*)) {
    if (!cfg || !scoreFunc) {
        return 1;
    }

    enigma_t enigma;
    enigma_t enigmaTmp;
    char*    plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        enigma.reflector = enigma_reflectors[i];
        memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));

        enigma_encode_string(&enigmaTmp, cfg->ciphertext, plaintext, cfg->ciphertext_length);
        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
    }

    free(plaintext);

    return 0;
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
 *
 * @return 0 on success, non-zero on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_rotor(
    enigma_crack_t* cfg, int targetRotor, float (*scoreFunc)(const enigma_crack_t*, const char*)) {
    if (!cfg || !scoreFunc || targetRotor > 3) {
        return 1;
    }

    enigma_t enigma;
    enigma_t enigmaTmp;
    char*    plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        enigma.rotors[targetRotor] = enigma_rotors[i];
        memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));
        enigma_encode_string(&enigmaTmp, cfg->ciphertext, plaintext, cfg->ciphertext_length);
        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
    }

    free(plaintext);

    return 0;
}

/**
 * @brief Crack the rotor order using a scoring function.
 *
 * This function attempts to determine the order of rotors used in the Enigma
 * machine by evaluating all possible rotor orders and scoring the resulting
 * plaintext using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 *
 * @return 0 on success, non-zero on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_crack_rotors(enigma_crack_t* cfg, float (*scoreFunc)(const enigma_crack_t*, const char*)) {
    if (!cfg || !scoreFunc) {
        return 1;
    }

    enigma_t enigma;
    enigma_t enigmaTmp;
    char*    plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
            if (i == j)
                continue;
            for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                if (j == k || i == k)
                    continue;

                if (enigma.rotor_count == 4) {
                    for (int l = 0; l < ENIGMA_ROTOR_COUNT; l++) {
                        if (i == l || j == l || k == l)
                            continue;

                        enigma.rotors[0] = enigma_rotors[i];
                        enigma.rotors[1] = enigma_rotors[j];
                        enigma.rotors[2] = enigma_rotors[k];
                        enigma.rotors[3] = enigma_rotors[l];
                        memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));

                        enigma_encode_string(&enigmaTmp,
                                             cfg->ciphertext,
                                             plaintext,
                                             cfg->ciphertext_length);
                        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
                    }
                } else {
                    enigma.rotors[0] = enigma_rotors[i];
                    enigma.rotors[1] = enigma_rotors[j];
                    enigma.rotors[2] = enigma_rotors[k];
                    memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));

                    enigma_encode_string(&enigmaTmp,
                                         cfg->ciphertext,
                                         plaintext,
                                         cfg->ciphertext_length);
                    enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
                }
            }
        }
    }

    free(plaintext);
    return 0;
}

/**
 * @brief Crack the rotor positions using a scoring function.
 *
 * This function attempts to determine the rotor starting positions of the
 * Enigma machine by evaluating all possible rotor positions and scoring the
 * resulting plaintext using the provided scoring function.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param scoreFunc Function pointer to the scoring function to use.
 *
 * @return 0 on success, non-zero otherwise.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_rotor_positions(enigma_crack_t* cfg,
                                                      float (*scoreFunc)(const enigma_crack_t*,
                                                                         const char*)) {
    if (!cfg || !scoreFunc) {
        return 1;
    }

    enigma_t enigma;
    enigma_t enigmaTmp;
    char*    plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
            for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                if (cfg->enigma.rotor_count == 4) {
                    for (int l = 0; l < ENIGMA_ALPHA_SIZE; l++) {
                        memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
                        enigma.rotor_indices[0] = i;
                        enigma.rotor_indices[1] = j;
                        enigma.rotor_indices[2] = k;
                        enigma.rotor_indices[3] = l;
                        memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));

                        enigma_encode_string(&enigmaTmp,
                                             cfg->ciphertext,
                                             plaintext,
                                             cfg->ciphertext_length);
                        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
                    }
                } else {
                    memcpy(&enigma, &cfg->enigma, sizeof(enigma_t));
                    enigma.rotor_indices[0] = i;
                    enigma.rotor_indices[1] = j;
                    enigma.rotor_indices[2] = k;
                    memcpy(&enigmaTmp, &enigma, sizeof(enigma_t));

                    enigma_encode_string(&enigmaTmp,
                                         cfg->ciphertext,
                                         plaintext,
                                         cfg->ciphertext_length);
                    enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
                }
            }
        }
    }

    free(plaintext);
    return 0;
}

/**
 * @brief Checks if multiple words from the dictionary are present in the
 * plaintext.
 *
 * This function checks the plaintext against a dictionary of words and returns
 * 1 if multiple words are found, otherwise returns 0.
 *
 * The dictionary must be uppercase.
 *
 * @param cfg The enigma_crack_t containing the dictionary and its size
 * @param plaintext The plaintext to check
 *
 * @return 1 if multiple words are found, 0 if not, -1 on error
 */
EMSCRIPTEN_KEEPALIVE int enigma_dict_match(const enigma_crack_t* cfg, const char* plaintext) {
    if (!cfg || !plaintext || !cfg->dictionary) {
        return -1;
    }

    int match_count = 0;
    for (size_t i = 0; i < cfg->dictionary_length; i++) {
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
 * @brief Finds potential indices in the ciphertext where the known plaintext
 * may exist.
 *
 * Due to the nature of the Enigma machine, a letter cannot be encoded to
 * itself. This function checks the ciphertext for potential indices where the
 * given string could potentially be in the plaintext.
 *
 * @param ciphertext The ciphertext to analyze
 * @param plaintext The known plaintext string to test against the ciphertext
 * @param indices Pointer to an array (length >= length of ciphertext) to store
 * the indices (-1-terminated)
 *
 * @return 0 on success, non-zero on failure
 */
EMSCRIPTEN_KEEPALIVE int
enigma_find_potential_indices(const char* ciphertext, const char* plaintext, int* indices) {
    if (!ciphertext || !plaintext || !indices) {
        return 1;
    }

    int matchCount    = 0;
    int plaintextLen  = strlen(plaintext);
    int ciphertextLen = strlen(ciphertext);

    for (int i = 0; i < ciphertextLen - plaintextLen + 1; i++) {
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
    return 0;
}

/**
 * @brief Calculate the frequency of characters in the plaintext.
 *
 * This function calculates the frequency of each character in the plaintext
 * and returns a score based on the frequencies.
 *
 * @param plaintext The plaintext to analyze
 * @param len The length of the plaintext
 * @return The frequency score of the plaintext, or -1 if the input is invalid.
 */
EMSCRIPTEN_KEEPALIVE float enigma_freq(const char* plaintext, int len) {
    if (!plaintext || len <= 0) {
        return -1;
    }
    int freq[26] = { 0 };
    for (int i = 0; i < len; i++) {
        if (plaintext[i] >= 'A' && plaintext[i] <= 'Z') {
            freq[plaintext[i] - 'A']++;
        }
    }

    float score = 0.0f;
    for (int i = 0; i < 26; i++) {
        score += (float) freq[i] * (freq[i] - 1);
    }
    return score / (len * (len - 1));
}

/**
 * @brief Check if the letter frequencies in the plaintext match target
 * frequencies within an offset.
 *
 * This function checks if the frequency of each letter in the plaintext is
 * within the specified offset of the target frequencies. If more than ten
 * letters deviate from the target frequencies, the function returns 0 (false).
 * Otherwise, it returns 1 (true).
 *
 * @param cfg       The enigma_crack_t containing the target frequencies
 * and offset
 * @param plaintext The plaintext to analyze
 *
 * @return 1 if over half of the letter frequencies match within the offset, 0
 * if not, -1 if error
 */
EMSCRIPTEN_KEEPALIVE int enigma_letter_freq(const enigma_crack_t* cfg, const char* plaintext) {
    if (!cfg || !plaintext) {
        return -1;
    }
    int letters[26] = { 0 };
    int nonMatching = 0;
    for (size_t i = 0; i < cfg->ciphertext_length; i++) {
        letters[plaintext[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        float freq = ((float) letters[i]) / cfg->ciphertext_length;
        if (freq < cfg->frequency_targets[i] - cfg->frequency_offset
            || freq > cfg->frequency_targets[i] + cfg->frequency_offset) {
            nonMatching++;
            if (nonMatching > 10) {
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
 * @param enigma Pointer to the enigma_t structure representing the scored
 * configuration.
 * @param plaintext The plaintext corresponding to the score.
 * @param score The score to append.
 * @return 0 on success, non-zero on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_score_append(enigma_crack_t* cfg, enigma_t* enigma, const char* plaintext, float score) {
    if (!cfg || !enigma || !plaintext) {
        return 1;
    }

    if (cfg->scores->score_count >= cfg->scores->max_scores) {
        cfg->scores->max_scores *= 2;
        cfg->scores->scores
            = realloc(cfg->scores->scores, cfg->scores->max_scores * sizeof(enigma_score_t));
    }

    memcpy(&cfg->scores->scores[cfg->scores->score_count].enigma, enigma, sizeof(enigma_t));
    cfg->scores->scores[cfg->scores->score_count].score = score;
    cfg->scores->scores[cfg->scores->score_count].flags = enigma_score_flags(cfg, plaintext);
    cfg->scores->score_count++;

    return 0;
}

/**
 * @brief Get the flags for a given plaintext based on the crack configuration.
 *
 * This function checks the plaintext against the criteria specified in the
 * enigma_crack_t and returns a bitmask of flags indicating which
 * criteria were met.
 *
 * @param cfg The enigma_crack_t containing the criteria and flags.
 * @param plaintext The plaintext to evaluate.
 * @return A bitmask of flags indicating which criteria were met, or -1 if
 * error.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_flags(const enigma_crack_t* cfg, const char* plaintext) {
    if (!cfg || !plaintext) {
        return -1;
    }
    int ret    = 0;
    int subRet = 0;

    if (cfg->flags & ENIGMA_FLAG_DICTIONARY_MATCH) {
        subRet = enigma_dict_match(cfg, plaintext);
        if (subRet == 1) {
            ret |= ENIGMA_FLAG_DICTIONARY_MATCH;
        } else if (subRet == -1) {
            return -1;
        }
    }

    if (cfg->flags & ENIGMA_FLAG_FREQUENCY) {
        float freqScore;
        int   freqRet = enigma_letter_freq(cfg, plaintext);

        if (freqRet == -1) {
            return -1;
        } else if (freqRet == 1) {
            ret |= ENIGMA_FLAG_FREQUENCY;
        }
    }

    if (cfg->flags & ENIGMA_FLAG_KNOWN_PLAINTEXT) {
        if (!cfg->known_plaintext) {
            return -1;
        }

        if (strstr(plaintext, cfg->known_plaintext)) {
            ret |= ENIGMA_FLAG_KNOWN_PLAINTEXT;
        }
    }
    return ret;
}
