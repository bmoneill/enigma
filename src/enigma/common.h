#ifndef ENIGMA_COMMON_H
#define ENIGMA_COMMON_H

#include "enigma.h"

#include <stdbool.h>

#define ENIGMA_ALPHA_SIZE             26
#define ENIGMA_MAX_CRIB_INDICES       256
#define ENIGMA_MAX_ROTOR_COUNT        4

/* Defaults */
#define ENIGMA_MAX_PLUGBOARD_SETTINGS 10
#define ENIGMA_DEFAULT_THREAD_COUNT   8
#define ENIGMA_DEFAULT_NGRAM_COUNT    100000

/* Cracking methods */
#define ENIGMA_MODE_BRUTE             0
#define ENIGMA_MODE_BOMBE             1
#define ENIGMA_MODE_BIGRAM            2
#define ENIGMA_MODE_TRIGRAM           3
#define ENIGMA_MODE_QUADGRAM          4
#define ENIGMA_MODE_IOC               5

/* Cracking targets */
#define ENIGMA_TARGET_ROTORS          0
#define ENIGMA_TARGET_POSITIONS       1
#define ENIGMA_TARGET_REFLECTOR       2
#define ENIGMA_TARGET_PLUGBOARD       3

/* Languages */
#define ENIGMA_LANG_ENGLISH           0
#define ENIGMA_LANG_GERMAN            1

#define VERBOSE_PRINT(fmt, ...)                \
    do {                                       \
        if (enigma_verbose) {                         \
            fprintf(stderr, fmt, __VA_ARGS__); \
        }                                      \
    } while (0)

#ifdef __GNUC__
#define ENIGMA_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ENIGMA_ALWAYS_INLINE inline
#endif


int enigma_load_reflector_config(enigma_t*, const char*);
int enigma_load_rotor_config(enigma_t*, char*);
int enigma_load_rotor_positions(enigma_t*, char*);

extern bool enigma_verbose;

#endif
