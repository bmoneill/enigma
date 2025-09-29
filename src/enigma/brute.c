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

static const enigma_crack_config_t* global_cfg = NULL;
static enigma_t* enigmas = NULL;
static char* plaintexts = NULL;
static pthread_t* threads = NULL;
static int threadCount = 0;

static void spawn(int,int);
static void* thread_main(void*);

/**
 * @brief Runs the brute force algorithm with the provided configuration and preset values.
 *
 * @param config The config structure containing the predefined Enigma settings, ciphertext, and
 *               other parameters
 */
void enigma_crack_brute(enigma_crack_config_t* crackCfg) {
    global_cfg = crackCfg;
    enigmas = malloc(global_cfg->maxThreads * sizeof(enigma_t));
    plaintexts = malloc(global_cfg->maxThreads * (global_cfg->ciphertextLen + 1) * sizeof(char));
    threads = malloc(global_cfg->maxThreads * sizeof(pthread_t));

    int flags = 0;
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_ROTOR_SETTINGS)) {
        flags |= FLAG_ROTORS;
    }
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_ROTOR_POSITIONS)) {
        flags |= FLAG_POSITIONS;
    }
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_REFLECTOR)) {
        flags |= FLAG_REFLECTOR;
    }
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_PLUGBOARD_SETTINGS)) {
        flags |= FLAG_PLUGBOARD;
    }

    crackCfg->flags = flags;

    memcpy(enigmas, &global_cfg->enigma, sizeof(enigma_t));

    thread_main((int[]){global_cfg->flags, 0});

    for (int t = 0; t < threadCount; t++) {
        pthread_join(threads[t], NULL);
    }

    free(threads);
    free(plaintexts);
    free(enigmas);
}

static void spawn(int flags, int threadNum) {
    int args[2];
    threadCount++;
    if (threadCount > global_cfg->maxThreads) {
        for (int t = 0; t < threadCount; t++) {
            if (t == threadNum) continue;
            pthread_join(threads[t], NULL);
        }
        threadCount = 0;
    }

    args[0] = flags;
    args[1] = threadCount - 1;

    memcpy(&enigmas[threadCount - 1], &enigmas[threadNum], sizeof(enigma_t));
    pthread_create(&threads[threadCount - 1], NULL, thread_main, args);
}

/**
 * @brief Entry point for each thread.
 *
 * This function performs the brute force search by iterating through all possible configurations
 * of the Enigma machine based on the flags provided. It decrypts the ciphertext and checks
 * against the dictionary or frequency analysis criteria.
 *
 * @param args An array containing the flags and thread number.
 * @return NULL
 */
static void* thread_main(void* args) {
    #define FLAGS ((int*)args)[0]
    #define THREADNUM ((int*)args)[1]
    #define MYENIGMA enigmas[THREADNUM]
    printf("Thread %d started with flags %d %d %d %d\n", THREADNUM, FLAGS & FLAG_ROTORS, FLAGS & FLAG_POSITIONS, FLAGS & FLAG_REFLECTOR, FLAGS & FLAG_PLUGBOARD);

    if (FLAGS & FLAG_ROTORS) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            memcpy(&MYENIGMA.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
            for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
                memcpy(&MYENIGMA.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
                for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                    if (i == j || j == k || i == k) continue;
                    memcpy(&MYENIGMA.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));

                    spawn(FLAGS & ~FLAG_ROTORS, THREADNUM);
                }
            }
        }
    } else if (FLAGS & FLAG_POSITIONS) {
        for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
            MYENIGMA.rotors[0].idx = i;
            for (int j = 0; j < ENIGMA_ALPHA_SIZE; j++) {
                MYENIGMA.rotors[1].idx = j;
                for (int k = 0; k < ENIGMA_ALPHA_SIZE; k++) {
                    MYENIGMA.rotors[2].idx = k;
                    spawn(FLAGS & ~FLAG_POSITIONS, THREADNUM);
                }
            }
        }
    } else if (FLAGS & FLAG_REFLECTOR) {
        for (int r = 0; r < ENIGMA_REFLECTOR_COUNT; r++) {
            memcpy(&MYENIGMA.reflector, enigma_reflectors[r], sizeof(enigma_reflector_t));
            spawn(FLAGS & ~FLAG_REFLECTOR, THREADNUM);
        }
    } else if (FLAGS & FLAG_PLUGBOARD) {
        // No plugboard
        spawn(FLAGS & ~FLAG_PLUGBOARD, THREADNUM);

        for (int i = 1; i < global_cfg->maxPlugboardSettings; i++) {
            for (int j = 0; j < i; j++) {
                for (int a = 0; a < ENIGMA_ALPHA_SIZE; a++) {
                    for (int b = 0; b < ENIGMA_ALPHA_SIZE; b++) {
                        if (a == b) continue;

                        MYENIGMA.plugboard[j * 2] = 'A' + a;
                        MYENIGMA.plugboard[j * 2 + 1] = 'A' + b;

                        spawn(FLAGS & ~FLAG_PLUGBOARD, THREADNUM);
                    }
                }

            }
        }
    } else {
        char* decrypted = &plaintexts[THREADNUM];
        enigma_encode_string(&MYENIGMA, global_cfg->ciphertext, decrypted, global_cfg->ciphertextLen);

        if (global_cfg->dictionary) {
            int match_count = enigma_dict_match(decrypted, global_cfg);
            if (match_count) {
                printf("%s %s %s %c%c%c %s %s: %s\n",
                       MYENIGMA.rotors[0].name, MYENIGMA.rotors[1].name, MYENIGMA.rotors[2].name,
                       MYENIGMA.rotors[0].idx, MYENIGMA.rotors[1].idx, MYENIGMA.rotors[2].idx,
                       MYENIGMA.reflector.name, MYENIGMA.plugboard, decrypted);
            }
        } else {
            if (enigma_letter_freq(decrypted, global_cfg->ciphertextLen, global_cfg->letterFreqTargets, global_cfg->letterFreqOffset)) {
                printf("%s %s %s %c%c%c %s %s: %s\n",
                       MYENIGMA.rotors[0].name, MYENIGMA.rotors[1].name, MYENIGMA.rotors[2].name,
                       MYENIGMA.rotors[0].idx, MYENIGMA.rotors[1].idx, MYENIGMA.rotors[2].idx,
                       MYENIGMA.reflector.name, MYENIGMA.plugboard, decrypted);
            }
        }
    }
    return NULL;
}
