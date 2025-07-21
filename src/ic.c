#include "ic.h"

#include "enigma.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void crack_plugboard(enigma_t *, const char *, char *, int, float);
static void crack_reflector(enigma_t *, const char *, char *, int, float);
static void crack_rotor_indexes(enigma_t *, const char *, char *, int, float);
static void crack_rotor_positions(enigma_t *, const char*, char*, int, float);
static void encode_string(enigma_t*, const char*, char*, int);
static void find_freq(int *, const char*, int len);
static float find_ic(float, int*, int);

const float enigma_ic_english_min = 0.05769; // 1.5/26
const float enigma_ic_english_max = 0.07692; // 2.0/26

/**
 * @brief Crack the Enigma machine using Index of Coincidence (IC).
 *
 * @param ciphertext The ciphertext to analyze.
 * @param length The length of the ciphertext.
 * @param min The minimum IC value for the language.
 * @param max The maximum IC value for the language.
 * @param offset The amount of potential configurations in each step to complete.
 * @param maxThreads The maximum number of threads to use for processing.
 */
void enigma_crack(const char *ciphertext, int length, float min, float max, float offset, int maxThreads) {
    pthread_t* threads = malloc(maxThreads * sizeof(pthread_t));
    int threadCount = 0;

    int freq[26] = {0};
    char *plaintext = malloc(length + 1);
    enigma_t enigma;

    enigma_init_default_config(&enigma);
    enigma.plugboard = "";

    crack_rotor_positions(&enigma, ciphertext, plaintext, length, target);
    crack_rotor_indexes(&enigma, ciphertext, plaintext, length, target);

    // Loop through all unique rotor and reflector configurations
    for (int i = 0; i < ROTOR_COUNT; i++) {
        memcpy(&enigma.rotors[0], enigma_rotors[i], sizeof(rotor_t));
        for (int j = 0; j < ROTOR_COUNT; j++) {
            if (i == j) continue;
            memcpy(&enigma.rotors[1], enigma_rotors[j], sizeof(rotor_t));
            for (int k = 0; k < ROTOR_COUNT; k++) {
                if (i == k || j == k) continue;
                memcpy(&enigma.rotors[2], enigma_rotors[k], sizeof(rotor_t));
                for (int l = 0; l < REFLECTOR_COUNT; l++) {
                    enigma.reflector = enigma_reflectors[l];
                    bombe_thread_args_t* args = malloc(sizeof(bombe_thread_args_t));
                    init_chunk_thread_args(args, bombe, &enigma, ciphertext, ciphertextLength);
                    pthread_create(&threads[threadCount++], NULL, thread_process_chunk, args);
                    if (threadCount >= maxThreads) {
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
 * @brief Initialize the bombe_thread_args_t structure with the given configuration.
 *
 * This function generates a bombe_thread_args_t that contains the arguments for
 * process_chunk(), in order to pass them to the created thread.
 */
static void init_chunk_thread_args(bombe_thread_args_t* dst,
    const bombe_t* bombe,
    const enigma_t* enigma,
    const char* ciphertext,
    int ciphertextLength) {
    memcpy(&dst->bombe, bombe, sizeof(bombe_t));
    memcpy(&dst->enigma, enigma, sizeof(enigma_t));
    dst->ciphertext = ciphertext;
    dst->ciphertextLength = ciphertextLength;
    dst->plaintext = malloc(ciphertextLength + 1);
    dst->plaintext[ciphertextLength] = '\0';
}

/**
 * @brief Process a rotor configuration chunk.
 *
 * Here we take a rotor, reflector, and plugboard configuration, minus rotor positions,
 * and loop through all possible rotor positions and check if any match our crib.
 *
 * @param bombe The bombe structure containing cribs and configuration
 * @param enigma The Enigma configuration to use for processing
 * @param ciphertext The ciphertext to analyze
 * @param ciphertextLength The length of the ciphertext
 * @param plaintext Buffer to store the decrypted plaintext (to avoid mallocing each time)
 */
static void process_chunk(bombe_t* bombe, enigma_t* enigma,
    const char* ciphertext, int ciphertextLength,
    char* plaintext) {
    char configString[256];

    for (int i = 0; i < ALPHA_SIZE; i++) {
        for (int j = 0; j < ALPHA_SIZE; j++) {
            for (int k = 0; k < ALPHA_SIZE; k++) {
                enigma->rotors[2].idx = i;
                enigma->rotors[1].idx = j;
                enigma->rotors[0].idx = k;
                sprintf(configString, "Rotors: %s (%c)  %s (%c), %s (%c) | Reflector: %s",
                    enigma->rotors[0].name, enigma->rotors[0].idx + 'A',
                    enigma->rotors[1].name, enigma->rotors[1].idx + 'A',
                    enigma->rotors[2].name, enigma->rotors[2].idx + 'A',
                    enigma->reflector->name);

                process_single(bombe, enigma, ciphertext, ciphertextLength, plaintext, configString);
            }
        }
    }
    free(plaintext);
}

/**
 * @brief Entry function for chunk processing threads
 *
 * This function is executed by each thread to process a chunk of the Bombe algorithm.
 *
 * @param args Pointer to the malloc()'d bombe_thread_args_t structure
 *             containing the arguments to process_chunk()
 *
 * @return NULL
 */
static void* thread_process_chunk(void* args) {
    bombe_thread_args_t* bombeArgs = (bombe_thread_args_t*)args;
    process_chunk(&bombeArgs->bombe, &bombeArgs->enigma, bombeArgs->ciphertext,
        bombeArgs->ciphertextLength, bombeArgs->plaintext);
    free(bombeArgs);
    return NULL;
}

/**
 * @brief Check the given enigma configuration against the ciphertext.
 *
 * This function checks if the given Enigma configuration results in a potential
 * plaintext. If it does, the function prints the configuration and plaintext to
 * stdout.
 *
 * @param bombe The bombe structure containing cribs and configuration
 * @param enigma The Enigma configuration to use for processing
 * @param ciphertext The ciphertext to analyze
 * @param ciphertextLength The length of the ciphertext
 * @param plaintext Buffer to store the decrypted plaintext
 * @param configString String representation of the current Enigma configuration
 */
static void process_single(bombe_t* bombe, enigma_t* enigma,
    const char* ciphertext, int ciphertextLength,
    char* plaintext, const char* configString) {

    int matching = 0;
    for (int i = 0; i < ciphertextLength; i++) {
        if (i > bombe->cribIndex && !matching) {
            // No match found
            return;
        }

        char decrypted = enigma_encode(enigma, ciphertext[i]);
        plaintext[i] = decrypted;

        if (matching == bombe->cribLength) {
            for (int j = i + 1; j < ciphertextLength; j++) {
                plaintext[j] = enigma_encode(enigma, ciphertext[j]);
            }
            break;
        } else if (i == bombe->cribIndex && decrypted == bombe->crib[0]) {
            matching = 1;
        } else if (matching && decrypted == bombe->crib[matching]) {
            matching++;
        } else if (matching && decrypted != bombe->crib[matching]) {
            return;
        }
    }

    printf("%s | Plaintext: %s\n", configString, plaintext);
}

static void encode_string(enigma_t* enigma, const char* input, char* output, int length) {
    for (int i = 0; i < length; i++) {
        output[i] = enigma_encode(enigma, input[i]);
    }
    output[length] = '\0';
}

float find_ic(float ic, int* freq, int length) {
    for (int i = 0; i < 26; i++) {
        ic += (float)freq[i] * (freq[i] - 1);
    }
    return ic / (float)(length * (length - 1));
}

void find_freq(int *dst, const char *text, int len) {
    for (int i = 0; i < len; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }

        dst[text[i] - 'A']++;
    }
}
