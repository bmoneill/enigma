/**
 * @file enigma/ngram.c
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

#define THREADNUM ((int*)args)[1]
#define MYENIGMA enigma_enigmas[THREADNUM]

static void  ngram_analyze        (int);
static void* reflector_thread_main(void*);
static void* rotor_thread_main    (void*);
static void* plugboard_thread_main(void*);
static void* positions_thread_main(void*);

const enigma_ngram_list_t* global_ngram_list = NULL;

/**
 * @brief Crack plugboard settings using n-gram scoring.
 *
 * This function attempts to determine the plugboard settings used in the ciphertext's Enigma key
 * by evaluating different plugboard combinations and scoring the resulting plaintext
 * using n-gram frequencies.
 *
 * Resulting scores and configurations are printed in descending order.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_plugboard_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    global_ngram_list = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))plugboard_thread_main);
}

/**
 * @brief Crack rotor configuration using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_rotors_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    global_ngram_list = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))rotor_thread_main);
}

/**
 * @brief Crack rotor positions using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_rotor_positions_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    global_ngram_list = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))positions_thread_main);
}

/**
 * @brief Crack reflector using n-gram scoring.
 *
 * @param config Pointer to the cracking configuration structure.
 * @param ngramList Pointer to the n-gram list structure.
 */
void enigma_crack_reflector_ngram(enigma_crack_config_t* config, enigma_ngram_list_t* ngramList) {
    global_ngram_list = ngramList;
    enigma_crack_multithreaded(config, (void* (*)(void*))reflector_thread_main);
}

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
 * @param threadnum The thread number corresponding to the decrypted text to analyze.
 */
static void ngram_analyze(int threadnum) {
    float score = 0.0f;
    const char* text = &enigma_plaintexts[threadnum];
    int textLen = enigma_global_cfg->ciphertextLen;

    switch (global_ngram_list->n) {
        case 2: score = enigma_bigram_score(text, enigma_global_cfg, (const float*)global_ngram_list->ngrams); break;
        case 3: score = enigma_trigram_score(text, enigma_global_cfg, (const float*)global_ngram_list->ngrams); break;
        case 4: score = enigma_quadram_score(text, enigma_global_cfg, (const float*)global_ngram_list->ngrams); break;
    }

    enigma_score_append(enigma_scores, score);
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
                if (i == j) continue;
                memcpy(&MYENIGMA.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
                for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                    if (j == k) continue;
                    memcpy(&MYENIGMA.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));
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
