/**
 * @file enigma/common.h
 *
 * This file contains common definitions, macros, and constants used throughout libenigma.
 */
#ifndef ENIGMA_COMMON_H
#define ENIGMA_COMMON_H

#define ENIGMA_ALPHA_SIZE      26
#define ENIGMA_MAX_ROTOR_COUNT 4

#define ENIGMA_SUCCESS         0
#define ENIGMA_FAILURE        -1

/* Defaults */
#define ENIGMA_MAX_PLUGBOARD_SETTINGS 10
#define ENIGMA_DEFAULT_NGRAM_COUNT    (26 * 26 * 26 * 26)

/* Languages */
#define ENIGMA_LANG_ENGLISH 0
#define ENIGMA_LANG_GERMAN  1

#ifndef LIBENIGMA_VERSION
#define LIBENIGMA_VERSION "unknown"
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
