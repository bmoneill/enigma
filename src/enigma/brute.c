#include "brute.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "io.h"
#include "rotors.h"
#include "reflectors.h"
#include "threads.h"

#define FLAG_REFLECTOR 1
#define FLAG_ROTORS 2
#define FLAG_POSITIONS 4
#define FLAG_PLUGBOARD 8

static void* thread_main(void*);
/**
 * @brief Runs the brute force algorithm with the provided configuration and preset values.
 *
 * @param config The config structure containing the predefined Enigma settings, ciphertext, and
 *               other parameters
 */
void enigma_crack_brute(enigma_crack_config_t* config) {
    enigma_crack_multithreaded(config, thread_main);
}

/**
 * @brief Entry point for each thread.
 *
 * This function performs the brute force search by iterating through all possible configurations
 * of the Enigma machine based on the flags provided. It decrypts the ciphertext and checks
 * against the dictionary if provided, printing any matches found.
 *
 * @param args An array containing the flags and thread number.
 * @return NULL
 */
static void* thread_main(void* args) {
    #define FLAGS ((int*)args)[0]
    #define THREADNUM ((int*)args)[1]
    #define MYENIGMA enigma_enigmas[THREADNUM]

    if (FLAGS & FLAG_ROTORS) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            memcpy(&MYENIGMA.rotors[0], enigma_rotors[i], sizeof(enigma_rotor_t));
            for (int j = 0; j < ENIGMA_ROTOR_COUNT; j++) {
                memcpy(&MYENIGMA.rotors[1], enigma_rotors[j], sizeof(enigma_rotor_t));
                for (int k = 0; k < ENIGMA_ROTOR_COUNT; k++) {
                    if (i == j || j == k || i == k) continue;
                    memcpy(&MYENIGMA.rotors[2], enigma_rotors[k], sizeof(enigma_rotor_t));

                    enigma_spawn(FLAGS & ~FLAG_ROTORS, THREADNUM);
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
                    enigma_spawn(FLAGS & ~FLAG_POSITIONS, THREADNUM);
                }
            }
        }
    } else if (FLAGS & FLAG_REFLECTOR) {
        for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
            memcpy(&MYENIGMA.reflector, enigma_reflectors[i], sizeof(enigma_reflector_t));
            enigma_spawn(FLAGS & ~FLAG_REFLECTOR, THREADNUM);
        }
    } else if (FLAGS & FLAG_PLUGBOARD) {
        // No plugboard
        enigma_spawn(0, THREADNUM);

        for (int i = 1; i < global_cfg->maxPlugboardSettings; i++) {
            printf("Spawning plugboard with %d pairs\n", i);
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
        char* decrypted = &enigma_plaintexts[THREADNUM * (global_cfg->ciphertextLen + 1)];
        char buf[80];
        int freq = 0;
        enigma_print_config(&MYENIGMA, buf);
        enigma_encode_string(&MYENIGMA, global_cfg->ciphertext, decrypted, global_cfg->ciphertextLen);

        if (global_cfg->dictionary) {
            int match_count = enigma_dict_match(decrypted, global_cfg);
            if (match_count) {
                printf("%s: %s\n", buf, decrypted);
            }
        } else {
            printf("%s: %s\n", buf, decrypted);
        }
    }

    enigma_freeThreads[THREADNUM] = 1;
    return NULL;
}
