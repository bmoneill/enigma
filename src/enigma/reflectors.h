/**
 * @file enigma/reflectors.h
 *
 * This file defines the standard reflectors used in the Enigma machine.
 */
#ifndef ENIGMA_REFLECTORS_H
#define ENIGMA_REFLECTORS_H

#include "common.h"

#define ENIGMA_REFLECTOR_COUNT 3

/**
 * @struct enigma_reflector_t
 * @brief Represents a reflector configuration for the Enigma machine.
 *
 * Indices represent the character code of each standard alphabetic character in the rotor's alphabet.
 *
 * @param name Name of the reflector.
 * @param indices Array of indices representing the reflector's wiring.
 */
typedef struct {
    const char* name;
    int         indices[ENIGMA_ALPHA_SIZE];
} enigma_reflector_t;

/**
 * @brief UKW-A reflector
 *
 * Alphabet: "EJMZALYXVBWFCRQUONTSPIKHGD"
 */
static const enigma_reflector_t enigma_UKW_A = {
    .name = "A",
    .indices
    = { 4, 9, 12, 25, 0, 1, 11, 23, 14, 19, 18, 17, 16, 15, 13, 10, 3, 2, 5, 6, 7, 8, 20, 21, 22 },
};

/**
 * @brief UKW-B reflector
 *
 * Alphabet: "YRUHQSLDPXNGOKMIEBFZCWVJAT"
 */
static const enigma_reflector_t enigma_UKW_B = {
    .name    = "B",
    .indices = { 24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14,
                 10, 12, 8,  4, 1,  5,  25, 2, 22, 21, 9,  0, 19 },
};

/**
 * @brief UKW-C reflector
 *
 * Alphabet: "FVPJIAOYEDRZXWGCTKUQSBNMHL"
 */
static const enigma_reflector_t enigma_UKW_C = {
    .name    = "C",
    .indices = { 5,  21, 15, 9,  8,  0,  14, 24, 4, 3,  17, 25, 23,
                 22, 6,  2,  19, 10, 20, 16, 18, 1, 13, 12, 7,  11 },
};

static const enigma_reflector_t* enigma_reflectors[] = {
    &enigma_UKW_A,
    &enigma_UKW_B,
    &enigma_UKW_C,
};

#endif
