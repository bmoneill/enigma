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

// Subset of enigma_crack_config_t for brute forcing
// Saves some memory by not including unnecessary fields
typedef struct {
    enigma_t             enigma;
    const char*          plaintext;
    int                  plaintextPos;
    int                  plaintextLen;
    const char*          ciphertext;
    int                  ciphertextLen;
    int                  maxPlugboardSettings;
    int                  maxThreads;
    const char*          dictionary;
    int                  flags;
} enigma_brute_config_t;

static int thread_count = 0;
static pthread_t* threads = NULL;

static void spawn(enigma_brute_config_t* cfg);
static void thread_main(enigma_brute_config_t* cfg);

/**
 * @brief Runs the brute force algorithm with the provided configuration and preset values.
 *
 * @param config The config structure containing the predefined Enigma settings, ciphertext, and
 *               other parameters
 */
void enigma_crack_brute(const enigma_crack_config_t* crackCfg) {
    enigma_brute_config_t* cfg = malloc(sizeof(enigma_brute_config_t));
    cfg->enigma = crackCfg->enigma;
    cfg->plaintext = crackCfg->plaintext;
    cfg->plaintextPos = crackCfg->plaintextPos;
    cfg->ciphertext = crackCfg->ciphertext;
    cfg->ciphertextLen = crackCfg->ciphertextLen;
    cfg->maxPlugboardSettings = crackCfg->maxPlugboardSettings;
    cfg->maxThreads = crackCfg->maxThreads;
    cfg->dictionary = crackCfg->dictionary;
    cfg->flags = crackCfg->flags;

    pthread_t* threads = malloc(cfg->maxThreads * sizeof(pthread_t));

    spawn(cfg);

    for (int t = 0; t < thread_count; t++) {
        pthread_join(threads[t], NULL);
    }
    free(threads);
}

static void spawn(enigma_brute_config_t* cfg) {
    pthread_create(&threads[thread_count++], NULL, thread_main, cfg);
    if (thread_count >= cfg->maxThreads) {
        for (int t = 0; t < thread_count; t++) {
            pthread_join(threads[t], NULL);
        }
        thread_count = 0;
    }
}

/**
 * @brief Entry point for each thread.
 */
static void thread_main(enigma_brute_config_t* cfg) {
    #define NEWCFG(cfg) memcpy(malloc(sizeof(enigma_brute_config_t)), cfg, sizeof(enigma_brute_config_t))
    enigma_brute_config_t* childCfg = NULL;

    if (cfg->flags & FLAG_ROTORS) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            memcpy(&cfg->enigma.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
            for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
                memcpy(&cfg->enigma.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
                for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                    if (i == j || j == k || i == k) continue;
                    memcpy(&cfg->enigma.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));

                    childCfg = NEWCFG(cfg);
                    childCfg->flags &= ~FLAG_ROTORS;
                    spawn(childCfg);
                }
            }
        }
    } else if (cfg->flags & FLAG_POSITIONS) {
        for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
            cfg->enigma.rotors[0].idx = i;
            for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
                cfg->enigma.rotors[1].idx = j;
                for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                    cfg->enigma.rotors[2].idx = k;

                    childCfg = NEWCFG(cfg);
                    childCfg->flags &= ~FLAG_POSITIONS;
                    spawn(childCfg);
                }
            }
        }
    } else if (cfg->flags & FLAG_REFLECTOR) {
        for (int r = 0; r < ENIGMA_REFLECTOR_COUNT; r++) {
            memcpy(&cfg->enigma.reflector, enigma_reflectors[r], sizeof(enigma_reflector_t));
            childCfg = NEWCFG(cfg);
            childCfg->flags &= ~FLAG_REFLECTOR;
            spawn(childCfg);
        }
    } else if (cfg->flags & FLAG_PLUGBOARD) {
        // No plugboard
        childCfg = NEWCFG(cfg);
        childCfg->flags &= ~FLAG_PLUGBOARD;
        spawn(childCfg);

        for (int i = 1; i < cfg->maxPlugboardSettings; i++) {
            for (int j = 0; j < i; j++) {
                for (int a = 0; a < ENIGMA_ALPHA_SIZE; a++) {
                    for (int b = 0; b < ENIGMA_ALPHA_SIZE; b++) {
                        if (a == b) continue;

                        cfg->enigma.plugboard[j * 2] = 'A' + a;
                        cfg->enigma.plugboard[j * 2 + 1] = 'A' + b;

                        childCfg = NEWCFG(cfg);
                        childCfg->flags &= ~FLAG_PLUGBOARD;
                        spawn(childCfg);
                    }
                }

            }
        }
    }
    else {
        char* decrypted = malloc(cfg->ciphertextLen + 1);
        enigma_encode_string(&cfg->enigma, cfg->ciphertext, decrypted, cfg->ciphertextLen);

        // TODO Decide via dictionary lookup or frequency analysis whether this
        // decryption is a possible candidate

        free(decrypted);
    }

    free(cfg);
}
