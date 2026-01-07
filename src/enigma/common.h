/**
 * @file enigma/common.h
 *
 * This file contains common definitions, macros, and constants used throughout libenigma.
 */
#ifndef ENIGMA_COMMON_H
#define ENIGMA_COMMON_H

/**
 * @brief Defines the size of the alphabet used in the Enigma machine.
 */
#define ENIGMA_ALPHA_SIZE 26

/**
 * @brief Defines the maximum number of rotors that can be used in the Enigma machine.
 */
#define ENIGMA_MAX_ROTOR_COUNT 4

/**
 * @brief Function executed successfully
 */
#define ENIGMA_SUCCESS 0

/**
 * @brief Function encountered an error
 */
#define ENIGMA_FAILURE -1

/* --- DEFAULTS --- */

/**
 * @brief Defines the maximum number of plugboard settings that can be used in the Enigma machine.
 */
#define ENIGMA_MAX_PLUGBOARD_SETTINGS 10

/**
 * @brief Defines the default number of n-grams used for analysis.
 *
 * 26^4 = 456976, the total number of possible 4-letter combinations.
 */
#define ENIGMA_DEFAULT_NGRAM_COUNT (26 * 26 * 26 * 26)

/* --- LANGUAGES --- */

/**
 * @brief Defines the language code for English.
 */
#define ENIGMA_LANG_ENGLISH 0

/**
 * @brief Defines the language code for German.
 */
#define ENIGMA_LANG_GERMAN 1

#ifndef ENIGMA_VERSION
/**
 * @brief Defines the version of libenigma.
 */
#define ENIGMA_VERSION "unknown"
#endif

#ifdef __GNUC__
#define ENIGMA_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ENIGMA_ALWAYS_INLINE inline
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#endif
