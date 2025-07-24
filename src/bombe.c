#include "bombe.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

typedef struct {
    bombe_t bombe;
    enigma_t enigma;
    const char* ciphertext;
    int ciphertextLength;
    char* plaintext;
} bombe_thread_args_t;

static float freq(const char* plaintext);
static void init_chunk_thread_args(bombe_thread_args_t*, const bombe_t*, const enigma_t*, const char*, int);
static void* thread_process_chunk(void*);
static void process_chunk(bombe_t*, enigma_t*, const char*, int, char*);
static void process_single(bombe_t*, enigma_t*, const char*, int, char*, const char*);

/**
 * @brief Initializes the Bombe structure with cribs and Enigma configuration.
 *
 * @param bombe The bombe structure to initialize
 * @param cribStrings Array of crib strings to be used
 * @param cribIndices Array of crib indices for each crib string
 * @param numCribs Number of crib strings (length of cribStrings and cribIndices)
 */
void enigma_bombe_init(bombe_t* bombe, char* crib, int cribIndex) {
    bombe->crib = crib;
    bombe->cribIndex = cribIndex;
    bombe->cribLength = strlen(crib);
}

/**
 * @brief Finds potential indices in the ciphertext where the crib could match.
 *
 * Due to the nature of the Enigma machine, a letter cannot be encoded to itself.
 * This function checks the ciphertext for potential indices where the crib string
 * could potentially be in the plaintext.
 *
 * @param ciphertext The ciphertext to analyze
 * @param crib The crib string to test against the ciphertext
 * @param indices Pointer to an array to store the indices (-1-terminated)
 */
void enigma_bombe_find_potential_indices(const char* ciphertext, const char* crib, int* indices) {
    int count = 0;
    int matchCount = 0;
    int cribLen = strlen(crib);
    int ciphertextLen = strlen(ciphertext);


    for (int i = 0; i < ciphertextLen - cribLen; i++) {
        for (int j = 0; j < cribLen; j++) {
            if (ciphertext[i + j] == crib[j]) {
                break;
            }
            if (j == cribLen - 1) {
                indices[matchCount++] = i;
            }
        }
    }

    indices[matchCount] = -1;
}

/**
 * @brief Runs the Bombe algorithm with the provided cribs and configuration.
 *
 * @param bombe The bombe structure containing cribs and configuration
 * @param ciphertext The ciphertext to analyze
 * @param maxThreads The number of threads to use for processing
 */
void enigma_bombe_run(bombe_t* bombe, const char* ciphertext, int maxThreads) {
    enigma_t enigma;
    int plugboard_count = 0;
    int ciphertextLength = strlen(ciphertext);

    pthread_t* threads = malloc(maxThreads * sizeof(pthread_t));
    int threadCount = 0;

    enigma_init_default_config(&enigma);


    // Loop through all unique rotor and reflector configurations
    for (int i = 0; i < ROTOR_COUNT; i++) {
        memcpy(&enigma.rotors[0], enigma_rotors[i], sizeof(rotor_t));
        for (int j = 0; j < ROTOR_COUNT; j++) {
            memcpy(&enigma.rotors[1], enigma_rotors[j], sizeof(rotor_t));
            for (int k = 0; k < ROTOR_COUNT; k++) {
                if (i == j || j == k || i == k) continue;
                memcpy(&enigma.rotors[2], enigma_rotors[k], sizeof(rotor_t));
                for (int l = 0; l < REFLECTOR_COUNT; l++) {
                    enigma.reflector = enigma_reflectors[l];

                    // free()'d at end of thread_process_chunk
                    // Sloppy, but necessary to avoid race condition
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

    printf("%f | %s | Plaintext: %s\n", freq(plaintext), configString, plaintext);
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
