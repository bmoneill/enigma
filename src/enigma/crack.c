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
#include "io.h"
#include "rotor.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int enigma_strcmp(const void*, const void*);

/**
 * @brief Create a new EnigmaCrackParams structure.
 *
 * This function allocates memory for a new EnigmaCrackParams structure.
 *
 * @return Pointer to the newly created EnigmaCrackParams structure, or NULL on failure.
 */
EMSCRIPTEN_KEEPALIVE EnigmaCrackParams* enigma_crack_params_new(void) {
    EnigmaCrackParams* params = malloc(sizeof(EnigmaCrackParams));
    if (!params) {
        return NULL;
    }
    return params;
}

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
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_plugboard(EnigmaCrackParams* cfg,
                                                float (*scoreFunc)(const EnigmaCrackParams*,
                                                                   const char*)) {
    if (!cfg || !scoreFunc) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    Enigma enigma = cfg->enigma;
    Enigma enigmaTmp;
    int    curSettings   = strlen(cfg->enigma.plugboard) / 2;

    char*  plaintext     = malloc((cfg->ciphertext_length + 1) * sizeof(char));
    int    remaining[26] = { 0 };

    memset(remaining, 1, sizeof(remaining));
    for (int i = 0; i < curSettings * 2; i++) {
        if (enigma.plugboard[i] < 'A' || enigma.plugboard[i] > 'Z') {
            return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        }
        remaining[toupper(enigma.plugboard[i]) - 'A'] = 0;
    }

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
            enigmaTmp                             = enigma;

            enigma_encode_string(&enigmaTmp, cfg->ciphertext, plaintext, cfg->ciphertext_length);
            enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
        }
    }

    free(plaintext);

    return ENIGMA_SUCCESS;
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
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_reflector(EnigmaCrackParams* cfg,
                                                float (*scoreFunc)(const EnigmaCrackParams*,
                                                                   const char*)) {
    if (!cfg || !scoreFunc) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    Enigma enigma;
    Enigma enigmaTmp;
    char*  plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    enigma           = cfg->enigma;
    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        enigma.reflector = enigma_reflectors[i];
        enigmaTmp        = enigma;

        enigma_encode_string(&enigmaTmp, cfg->ciphertext, plaintext, cfg->ciphertext_length);
        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
    }

    free(plaintext);

    return ENIGMA_SUCCESS;
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
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_rotor(EnigmaCrackParams* cfg,
                                            int                targetRotor,
                                            float (*scoreFunc)(const EnigmaCrackParams*,
                                                               const char*)) {
    if (!cfg || !scoreFunc || targetRotor > 3) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    Enigma enigma;
    Enigma enigmaTmp;
    char*  plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    enigma           = cfg->enigma;
    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        enigma.rotors[targetRotor] = enigma_rotors[i];
        enigmaTmp                  = enigma;
        enigma_encode_string(&enigmaTmp, cfg->ciphertext, plaintext, cfg->ciphertext_length);
        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
    }

    free(plaintext);

    return ENIGMA_SUCCESS;
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
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_rotors(EnigmaCrackParams* cfg,
                                             float (*scoreFunc)(const EnigmaCrackParams*,
                                                                const char*)) {
    if (!cfg || !scoreFunc) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    Enigma enigma;
    Enigma enigmaTmp;
    char*  plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    enigma           = cfg->enigma;
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
                        enigmaTmp        = enigma;

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
                    enigmaTmp        = enigma;

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
    return ENIGMA_SUCCESS;
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
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_rotor_positions(EnigmaCrackParams* cfg,
                                                      float (*scoreFunc)(const EnigmaCrackParams*,
                                                                         const char*)) {
    if (!cfg || !scoreFunc) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    Enigma enigma;
    Enigma enigmaTmp;
    char*  plaintext = malloc((cfg->ciphertext_length + 1) * sizeof(char));

    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
            for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                if (cfg->enigma.rotor_count == 4) {
                    for (int l = 0; l < ENIGMA_ALPHA_SIZE; l++) {
                        enigma                  = cfg->enigma;
                        enigma.rotor_indices[0] = i;
                        enigma.rotor_indices[1] = j;
                        enigma.rotor_indices[2] = k;
                        enigma.rotor_indices[3] = l;
                        enigmaTmp               = enigma;

                        enigma_encode_string(&enigmaTmp,
                                             cfg->ciphertext,
                                             plaintext,
                                             cfg->ciphertext_length);
                        enigma_score_append(cfg, &enigma, plaintext, scoreFunc(cfg, plaintext));
                    }
                } else {
                    enigma                  = cfg->enigma;
                    enigma.rotor_indices[0] = i;
                    enigma.rotor_indices[1] = j;
                    enigma.rotor_indices[2] = k;
                    enigmaTmp               = enigma;

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
    return ENIGMA_SUCCESS;
}

/**
 * @brief Checks if multiple words from the dictionary are present in the
 * plaintext.
 *
 * This function checks the plaintext against a dictionary of words and returns
 * 1 if multiple words are found, otherwise returns 0.
 *
 * The dictionary must be uppercase and sorted.
 *
 * @param cfg The EnigmaCrackParams struct containing the dictionary and its size
 * @param plaintext The plaintext to check
 *
 * @return 1 if multiple words are found, 0 if not, `ENIGMA_FAILURE` on error
 */
EMSCRIPTEN_KEEPALIVE int enigma_dict_match(const EnigmaCrackParams* cfg, const char* plaintext) {
    if (!cfg || !plaintext || !cfg->dictionary) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    int match_count = 0;

#if 0
    // strstr implementation
    for (size_t i = 0; i < cfg->dictionary_length; i++) {
        if (strstr(plaintext, cfg->dictionary[i]) != NULL) {
            match_count++;
            if (match_count > 1) {
                return 1;
            }
        }
    }
#else
    // bsearch implementation
    for (size_t i = 0; i < cfg->ciphertext_length; i++) {
        void* result = bsearch(&plaintext[i],
                               cfg->dictionary,
                               cfg->dictionary_length,
                               sizeof(const char*),
                               enigma_strcmp);
        if (result != NULL) {
            match_count++;
            if (match_count > 1) {
                return 1;
            }
        }
    }
#endif
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
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_find_potential_indices(const char* ciphertext, const char* plaintext, int* indices) {
    if (!ciphertext || !plaintext || !indices) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
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
    return ENIGMA_SUCCESS;
}

/**
 * @brief Calculate the frequency of characters in the plaintext.
 *
 * This function calculates the frequency of each character in the plaintext
 * and returns a score based on the frequencies.
 *
 * @param plaintext The plaintext to analyze
 * @param len The length of the plaintext
 * @return The frequency score of the plaintext, or `ENIGMA_FAILURE` if the input is invalid.
 */
EMSCRIPTEN_KEEPALIVE float enigma_freq(const char* plaintext, int len) {
    if (!plaintext || len <= 0) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
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
 * @param cfg       The EnigmaCrackParams struct containing the target frequencies
 * and offset
 * @param plaintext The plaintext to analyze
 *
 * @return 1 if over half of the letter frequencies match within the offset, 0
 * if not, `ENIGMA_FAILURE` if error.
 */
EMSCRIPTEN_KEEPALIVE int enigma_letter_freq(const EnigmaCrackParams* cfg, const char* plaintext) {
    if (!cfg || !plaintext) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
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
 * @brief Append a score to an EnigmaScoreList.
 *
 * If the scores array is full, it will be resized to double its current size.
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param enigma Pointer to the Enigma structure representing the scored
 * configuration.
 * @param plaintext The plaintext corresponding to the score.
 * @param score The score to append.
 * @return `ENIGMA_SUCCESS` on success, `ENIGMA_FAILURE` on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_score_append(EnigmaCrackParams* cfg, Enigma* enigma, const char* plaintext, float score) {
    if (!cfg || !enigma || !plaintext) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    if (cfg->score_list->score_count >= cfg->score_list->max_scores) {
        cfg->score_list->max_scores *= 2;
        cfg->score_list->scores
            = realloc(cfg->score_list->scores, cfg->score_list->max_scores * sizeof(EnigmaScore));
    }

    cfg->score_list->scores[cfg->score_list->score_count].enigma = *enigma;
    cfg->score_list->scores[cfg->score_list->score_count].score  = score;
    cfg->score_list->scores[cfg->score_list->score_count].flags
        = enigma_score_flags(cfg, plaintext);
    cfg->score_list->score_count++;

    return ENIGMA_SUCCESS;
}

/**
 * @brief Get the flags for a given plaintext based on the crack configuration.
 *
 * This function checks the plaintext against the criteria specified in the
 * EnigmaCrackParams and returns a bitmask of flags indicating which
 * criteria were met.
 *
 * @param cfg The EnigmaCrackParams containing the criteria and flags.
 * @param plaintext The plaintext to evaluate.
 * @return A bitmask of flags indicating which criteria were met, or `ENIGMA_FAILURE` if
 * error.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_flags(const EnigmaCrackParams* cfg, const char* plaintext) {
    if (!cfg || !plaintext) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
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

/**
 * @brief Get the enigma field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The enigma field, or NULL if cfg is NULL.
 */
EMSCRIPTEN_KEEPALIVE const Enigma* enigma_crack_get_enigma(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return &cfg->enigma;
}

/**
 * @brief Get the score_list field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The scores field, or NULL if cfg is NULL.
 */
EMSCRIPTEN_KEEPALIVE const EnigmaScoreList*
enigma_crack_get_score_list(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return cfg->score_list;
}

/**
 * @brief Get the dictionary field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The dictionary field, or NULL if cfg is NULL.
 */
EMSCRIPTEN_KEEPALIVE const char** enigma_crack_get_dictionary(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return cfg->dictionary;
}

/**
 * @brief Get the `dictionary_length` field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The `dictionary_length` field, or `ENIGMA_FAILURE` if `cfg` is NULL.
 */
EMSCRIPTEN_KEEPALIVE size_t enigma_crack_get_dictionary_length(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->dictionary_length;
}

/**
 * @brief Get the ngrams field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The `ngrams` field, or NULL if cfg is NULL.
 */
EMSCRIPTEN_KEEPALIVE const float* enigma_crack_get_ngrams(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return cfg->ngrams;
}

/**
 * @brief Get the n field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The n field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_get_n(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->n;
}

/**
 * @brief Get the ngrams_length field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The ngrams_length field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE size_t enigma_crack_get_ngrams_length(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->ngrams_length;
}

/**
 * @brief Get the ciphertext field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The ciphertext field, or NULL if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_crack_get_ciphertext(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return cfg->ciphertext;
}

/**
 * @brief Get the ciphertext_length field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The ciphertext_length field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE size_t enigma_crack_get_ciphertext_length(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->ciphertext_length;
}

/**
 * @brief Get the flags field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The flags field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_get_flags(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->flags;
}

/**
 * @brief Get the frequency_targets field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The frequency_targets field, or NULL if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE const float* enigma_crack_get_frequency_targets(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return cfg->frequency_targets;
}

/**
 * @brief Get the min_score field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The min_score field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE float enigma_crack_get_min_score(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return (float) ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->min_score;
}

/**
 * @brief Get the max_score field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The max_score field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE float enigma_crack_get_max_score(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return (float) ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->max_score;
}

/**
 * @brief Get the target_score field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The target_score field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE float enigma_crack_get_target_score(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return (float) ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->target_score;
}

/**
 * @brief Get the known_plaintext field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The known_plaintext field, or NULL if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_crack_get_known_plaintext(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return cfg->known_plaintext;
}

/**
 * @brief Get the known_plaintext_length field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @return The known_plaintext_length field, or ENIGMA_FAILURE if cfg is NULL
 */
EMSCRIPTEN_KEEPALIVE size_t enigma_crack_get_known_plaintext_length(const EnigmaCrackParams* cfg) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return cfg->known_plaintext_length;
}

/**
 * @brief Set the enigma field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param enigma The Enigmainstance
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_enigma(EnigmaCrackParams* cfg, Enigma* enigma) {
    if (!cfg || !enigma) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->enigma = *enigma;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the score_list field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param scoreList The EnigmaScoreList instance
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_score_list(EnigmaCrackParams* cfg,
                                                     EnigmaScoreList*   scoreList) {
    if (!cfg || !scoreList) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->score_list = scoreList;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the dictionary field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param dictionary The dictionary array
 * @param length The length of the dictionary array
 * @return 0 on success, -1 on failure
 */
EMSCRIPTEN_KEEPALIVE int
enigma_crack_set_dictionary(EnigmaCrackParams* cfg, const char** dictionary, size_t length) {
    if (!cfg || !dictionary) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->dictionary        = dictionary;
    cfg->dictionary_length = length;
    return 0;
}

/**
 * @brief Set the ngrams field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param ngrams The ngrams array
 * @param n The n value
 * @param length The length of the ngrams array
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int
enigma_crack_set_ngrams(EnigmaCrackParams* cfg, float* ngrams, int n, size_t length) {
    if (!cfg || !ngrams || n < 2 || n > 4) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->ngrams        = ngrams;
    cfg->n             = n;
    cfg->ngrams_length = length;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the n field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param n The n value
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_n(EnigmaCrackParams* cfg, int n) {
    if (!cfg || n < 2 || n > 4) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->n = n;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the ciphertext field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param ciphertext The ciphertext string
 * @param length The length of the ciphertext string
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int
enigma_crack_set_ciphertext(EnigmaCrackParams* cfg, const char* ciphertext, size_t length) {
    if (!cfg || !ciphertext) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->ciphertext        = ciphertext;
    cfg->ciphertext_length = length;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the flags field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param flags The flags value
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_flags(EnigmaCrackParams* cfg, int flags) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->flags = flags;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the frequency targets field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param frequencyTargets The frequency targets array
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_frequency_targets(EnigmaCrackParams* cfg,
                                                            float*             frequencyTargets) {
    if (!cfg || !frequencyTargets) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    memcpy(cfg->frequency_targets, frequencyTargets, sizeof(float) * 26);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the minimum score field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param minScore The minimum score value
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_min_score(EnigmaCrackParams* cfg, float minScore) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->min_score = minScore;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the maximum score field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param maxScore The maximum score value
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_max_score(EnigmaCrackParams* cfg, float maxScore) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->max_score = maxScore;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the target score field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param targetScore The target score value
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_target_score(EnigmaCrackParams* cfg, float targetScore) {
    if (!cfg) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->target_score = targetScore;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the known plaintext field in the given EnigmaCrackParams struct
 *
 * @param cfg The EnigmaCrackParams struct instance
 * @param knownPlaintext The known plaintext string
 * @param length The length of the known plaintext string
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_crack_set_known_plaintext(EnigmaCrackParams* cfg,
                                                          const char*        knownPlaintext,
                                                          size_t             length) {
    if (!cfg || !knownPlaintext) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    cfg->known_plaintext        = knownPlaintext;
    cfg->known_plaintext_length = length;
    return ENIGMA_SUCCESS;
}

static int enigma_strcmp(const void* s1, const void* s2) {
    return strcmp((const char*) s1, (const char*) s2);
}
