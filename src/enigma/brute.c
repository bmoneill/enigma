#include "brute.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

#define FLAG_REFLECTOR 1
#define FLAG_ROTORS 2
#define FLAG_POSITIONS 4
#define FLAG_PLUGBOARD 8

typedef struct {
    enigma_crack_config_t config;
    int                   flag;
} brute_thread_args_t;

/* Changed the struct here, use enigma_crack_config_t instead of bombe_t. Need to fix all these errors. */
static float freq(const char* plaintext);
static void init_chunk_thread_args(brute_thread_args_t*, const enigma_crack_config_t*, int);
static void* thread_process_chunk(void*);
static void process_chunk(enigma_crack_config_t*, enigma_t*, const char*, int, char*);
static void process_single(enigma_crack_config_t*, enigma_t*, const char*, int, char*, const char*);

/**
 * @brief Runs the brute force algorithm with the provided configuration and preset values.
 *
 * @param config The config structure containing the predefined Enigma settings, ciphertext, and
 *               other parameters
 */
void enigma_crack_brute(const enigma_crack_config_t* config) {
    enigma_t enigma;
    int ciphertextLength = strlen(config->ciphertext);

    enigma_crack_config_t *config_copy = malloc(sizeof(enigma_crack_config_t));
    memcpy(config_copy, config, sizeof(enigma_crack_config_t));

    pthread_t* threads = malloc(config->maxThreads * sizeof(pthread_t));
    int threadCount = 0;

    int count = 0;

    // Loop through all unique rotor and reflector configurations
    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        memcpy(&enigma.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
        for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
            memcpy(&enigma.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
            for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                if (i == j || j == k || i == k) continue;
                memcpy(&enigma.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));
                for (int l = 0; l < ENIGMA_REFLECTOR_COUNT; l++) {
                    memcpy(&enigma.reflector, enigma_reflectors[l], sizeof(enigma_reflector_t));

                    // free()'d at end of thread_process_chunk
                    // Sloppy, but necessary to avoid race condition
                    enigma_crack_config_t* args = malloc(sizeof(enigma_crack_config_t));

                    init_chunk_thread_args(args, config);
                    pthread_create(&threads[threadCount++], NULL, thread_process_chunk, args);
                    if (threadCount >= config->maxThreads) {
                        for (int t = 0; t < threadCount; t++) {
                            pthread_join(threads[t], NULL);
                        }
                        threadCount = 0;
                    }
                }
            }
        }
    }

    for (int t = 0; t < threadCount; t++) {
        pthread_join(threads[t], NULL);
    }
    free(threads);
}

/**
 * @brief Initialize the brute_thread_args_t structure with the given configuration.
 *
 * This function generates a brute_thread_args_t that contains the arguments for
 * process_chunk(), in order to pass them to the created thread.
 *
 * The flag argument indicates what part of the Enigma configuration to vary.
 *
 * @param dst Pointer to the destination brute_thread_args_t structure.
 * @param config Pointer to the source enigma_crack_config_t structure.
 * @param flag Which part of the Enigma configuration to vary (FLAG_REFLECTOR, FLAG_ROTORS,
 *             FLAG_POSITIONS, or FLAG_PLUGBOARD).
 */
static void init_chunk_thread_args(brute_thread_args_t* dst,
                                   const enigma_crack_config_t* config, int flag) {
    memcpy(&dst->config, config, sizeof(enigma_crack_config_t));
    dst->flag = flag;
    dst->config.plaintext = malloc(config->ciphertextLen + 1);
    dst->config.plaintext[config->ciphertextLen] = '\0';
}


static void run_thread(enigma_crack_config_t* config, int flag) {
    enigma_crack_config_t* args = malloc(sizeof(enigma_crack_config_t));
    init_chunk_thread_args(args, config, flag);
    pthread_create(&threads[threadCount++], NULL, thread_process_chunk, args);
    if (threadCount >= maxThreads) {
        for (int t = 0; t < threadCount; t++) {
            pthread_join(threads[t], NULL);
        }
        threadCount = 0;
    }
}

/**
 * @brief Process a rotor configuration chunk.
 */
static void process_chunk(brute_thread_args_t* cfg) {
    char configString[256];

    }
    free(plaintext);
}

/**
 * @brief Entry function for chunk processing threads
 *
 * This function is executed by each thread to process a chunk of the Bombe algorithm.
 *
 * @param args Pointer to the malloc()'d bombe_thread_args_t structure containing the arguments to
 *             process_chunk()
 *
 * @return NULL
 */
static void* thread_process_chunk(void* args) {
    enigma_crack_config_t* config = (enigma_crack_config_t*)args;
    process_chunk(&config->enigma, config->ciphertext, config->ciphertextLength, config->plaintext);
    free(config);
    return NULL;
}

static void process_plugboard(enigma_crack_config_t* cfg) {
    for (int i = 0; i < cfg->maxPlugboardSettings; i++) {
        for (int j = 0; j < i; j += 2) {
            for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                for (int l = 0; l < ENIGMA_ALPHA_SIZE; l++) {
                    if (k == l) continue;
                    cfg->enigma.plugboard[j] = l + 'A';
                    cfg->enigma.plugboard[j+1] = k + 'A';
                    process_single(cfg);
                }
            }
        }
    }
}

static void process_rotors(enigma_crack_config_t* cfg) {
    for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
        memcpy(&cfg->enigma.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
        for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
            memcpy(&cfg->enigma.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
            for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                if (i == j || j == k || i == k) continue;
                memcpy(&cfg->enigma.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));
                // TODO figure out if we need to loop through rotor positions here
                // Should also be multithreaded
            }
        }
    }
}

static void process_positions(enigma_crack_config_t* cfg) {
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
            for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                cfg->enigma.rotors[0].idx = i;
                cfg->enigma.rotors[1].idx = j;
                cfg->enigma.rotors[2].idx = k;
                process_positions(cfg);
            }
        }
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
static float freq(const char* plaintext) {
    int freq[26] = {0};
    int len = strlen(plaintext);
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

