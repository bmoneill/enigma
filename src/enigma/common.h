#ifndef ENIGMA_COMMON_H
#define ENIGMA_COMMON_H

#include <stdbool.h>

#define ENIGMA_ALPHA_SIZE             26
#define ENIGMA_MAX_CRIB_INDICES       256
#define ENIGMA_MAX_ROTOR_COUNT        4

/* Defaults */
#define ENIGMA_MAX_PLUGBOARD_SETTINGS 10
#define ENIGMA_DEFAULT_THREAD_COUNT   8
#define ENIGMA_DEFAULT_NGRAM_COUNT    100000

/* Cracking methods */
#define ENIGMA_METHOD_BRUTE           0
#define ENIGMA_METHOD_BOMBE           1
#define ENIGMA_METHOD_NGRAM           2
#define ENIGMA_METHOD_IOC             3

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

static bool enigma_verbose = false;

#endif
