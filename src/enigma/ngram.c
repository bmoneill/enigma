#include "ngram.h"

#include "common.h"
#include "crack.h"
#include "io.h"
#include "threads.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIIDX(a, b) ((a << 5) | b)
#define TRIIDX(a, b, c) ((a << 10) | (b << 5) | c)
#define QUADIDX(a, b, c, d) ((a << 15) | (b << 10) | (c << 5) | d)

#define THREADNUM ((int*)args)[1]
#define MYENIGMA enigma_enigmas[THREADNUM]

const enigma_ngram_list_t* globalNgramList = NULL;

static void ngram_analyze(int threadnum);
static void* reflector_thread_main(void* args);
static void* rotor_thread_main(void* args);
static void* plugboard_thread_main(void* args);
static void* positions_thread_main(void* args);

/**
 * @brief Crack plugboard settings using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_plugboard_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    globalNgramList = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))plugboard_thread_main);
}

/**
 * @brief Crack rotor configuration using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_rotors_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    globalNgramList = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))rotor_thread_main);
}

/**
 * @brief Crack rotor positions using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_rotor_positions_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    globalNgramList = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))positions_thread_main);
}

/**
 * @brief Crack reflector using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 * @todo Implement
 */
void enigma_crack_reflector_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    globalNgramList = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))reflector_thread_main);
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

/**
 * @brief Analyze decrypted text using n-gram scoring.
 *
 * This function scores the decrypted text using the n-gram method
 * and prints the configuration and score if it falls within the specified range.
 *
 * @param threadnum The thread number corresponding to the decrypted text to analyze.
 *
 * @todo We should store results in a list and sort them by score before printing.
 */
static void ngram_analyze(int threadnum) {
    float score = 0.0f;
    const char* text = &enigma_plaintexts[threadnum];
    int textLen = enigma_global_cfg->ciphertextLen;

    switch (globalNgramList->n) {
        case 2: score = enigma_bigram_score(text, textLen, (const float*)globalNgramList->ngrams); break;
        case 3: score = enigma_trigram_score(text, textLen, (const float*)globalNgramList->ngrams); break;
        case 4: score = enigma_quadram_score(text, textLen, (const float*)globalNgramList->ngrams); break;
    }

    if (score >= enigma_global_cfg->minScore && score <= enigma_global_cfg->maxScore) {
        char buf[80];
        enigma_print_config(&enigma_enigmas[threadnum], buf);
        printf("%.6f\t%s\t%s\n", score, buf, &enigma_plaintexts[threadnum]);
    }
}

/**
 * @brief Thread main function for plugboard cracking.
 *
 * This function iterates through all possible plugboard settings with up to
 * the maximum number of settings specified in the global configuration.
 * It evaluates the resulting plaintext using n-gram scoring.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* plugboard_thread_main(void* args) {
    if (THREADNUM == 0) {
        enigma_spawn(0, THREADNUM);

        for (int i = 1; i < enigma_global_cfg->maxPlugboardSettings; i++) {
            for (int j = 0; j < i; j++) {
                for (int a = 0; a < ENIGMA_ALPHA_SIZE; a++) {
                    for (int b = 0; b < ENIGMA_ALPHA_SIZE; b++) {
                        if (a == b) continue;

                        MYENIGMA.plugboard[j * 2] = 'A' + a;
                        MYENIGMA.plugboard[j * 2 + 1] = 'A' + b;

                        enigma_spawn(0, THREADNUM);
                    }
                }
            }
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        ngram_analyze(THREADNUM);
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

/**
 * @brief Thread main function for rotor cracking.
 *
 * This function iterates through all possible rotor combinations
 * and evaluates the resulting plaintext using n-gram scoring.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* positions_thread_main(void* args) {
    if (THREADNUM == 0) {
        for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
            MYENIGMA.rotors[0].idx = i;
            for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
                MYENIGMA.rotors[1].idx = j;
                for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                    MYENIGMA.rotors[2].idx = k;
                    enigma_spawn(0, 0);
                }
            }
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        ngram_analyze(THREADNUM);
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

/**
 * @brief Thread main function for reflector cracking.
 *
 * This function iterates through all possible reflectors
 * and evaluates the resulting plaintext using n-gram scoring.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* reflector_thread_main(void* args) {
    if (THREADNUM == 0) {
        for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
            memcpy(&MYENIGMA.reflector, &enigma_reflectors[i], sizeof(enigma_reflector_t));
            enigma_spawn(0, 0);
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        ngram_analyze(THREADNUM);
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}

/**
 * @brief Thread main function for rotor cracking.
 *
 * This function iterates through all possible rotor combinations
 * and evaluates the resulting plaintext using n-gram scoring.
 *
 * @param args Integer thread arguments, where the second element is the thread number.
 * @return NULL
 */
static void* rotor_thread_main(void* args) {
    if (THREADNUM == 0) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            memcpy(&MYENIGMA.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
            for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
                if (i == j) {
                    continue;
                }
                memcpy(&MYENIGMA.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
                for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                    memcpy(&MYENIGMA.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));
                    if (j == k) {
                        continue;
                    }

                    enigma_spawn(0, 0);
                }
            }
        }
    } else {
        enigma_encode_string(&MYENIGMA, enigma_global_cfg->ciphertext, &enigma_plaintexts[THREADNUM], enigma_global_cfg->ciphertextLen);
        ngram_analyze(THREADNUM);
    }
    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}
