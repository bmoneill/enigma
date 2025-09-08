#ifndef ENIGMA_ROTORS_H
#define ENIGMA_ROTORS_H

#include "common.h"

#define ENIGMA_ROTOR_COUNT 8

typedef struct {
    const char* name;
    int fwd_indices[ENIGMA_ALPHA_SIZE];
    int rev_indices[ENIGMA_ALPHA_SIZE];
    int notches[2];
    int numNotches;
    int idx;
} enigma_rotor_t;

// Indices here are entered manually to avoid extra calculation at runtime.
// Essentially, they map the rotor's alphabet to the standard alphabet (A-Z).
// fwd_indices represents the character code of each standard alphabetic character in the rotor's alphabet.
// rev_indices represents the character code of each rotor's alphabetic character in the standard alphabet.

// Alphabet: "EKMFLGDQVZNTOWYHXUSPAIBRCJ"
// Notches: Q
static const enigma_rotor_t enigma_rotor_I = {
    .name = "I",
    .fwd_indices = { 4, 10, 12, 5, 11, 6, 3, 16, 21, 25, 13, 19, 14, 22, 24, 7, 23, 20, 18, 15, 0, 8, 1, 17, 2, 9 },
    .rev_indices = { 20, 22, 24, 6, 0, 3, 5, 15, 21, 25, 1, 4, 2, 10, 12, 19, 7, 23, 18, 11, 17, 8, 13, 16, 14, 9 },
    .notches = { 16 },
    .numNotches = 1,
    .idx = 0,
};

// Alphabet: "AJDKSIRUXBLHWTMCQGZNPYFVOE"
// Notches: E
static const enigma_rotor_t enigma_rotor_II = {
    .name = "II",
    .fwd_indices = { 0, 9, 3, 10, 18, 8, 17, 20, 23, 1, 11, 7, 22, 19, 12, 2, 16, 6, 25, 13, 15, 24, 5, 21, 14, 4 },
    .rev_indices = { 0, 9, 15, 2, 25, 22, 17, 11, 5, 1, 3, 10, 14, 19, 24, 20, 16, 6, 4, 13, 7, 23, 12, 8, 21, 18 },
    .notches = { 4 },
    .numNotches = 1,
    .idx = 0,
};

// Alphabet: "BDFHJLCPRTXVZNYEIWGAKMUSQO"
// Notches: V
static const enigma_rotor_t enigma_rotor_III = {
    .name = "III",
    .fwd_indices = { 1, 3, 5, 7, 9, 11, 2, 15, 17, 19, 23, 21, 25, 13, 24, 4, 8, 22, 6, 0, 10, 12, 20, 18, 16, 14 },
    .rev_indices = { 19, 0, 6, 1, 15, 2, 18, 3, 16, 4, 20, 5, 21, 13, 25, 7, 24, 8, 23, 9, 22, 11, 17, 10, 14, 12 },
    .notches = { 21 },
    .numNotches = 1,
    .idx = 0,
};

// Alphabet: "ESOVPZJAYQUIRHXLNFTGKDCMWB"
// Notches: R
static const enigma_rotor_t enigma_rotor_IV = {
    .name = "IV",
    .fwd_indices = { 4, 18, 14, 21, 15, 25, 9, 0, 24, 16, 20, 8, 17, 7, 23, 11, 13, 5, 19, 6, 10, 3, 2, 12, 22, 1 },
    .rev_indices = { 7, 25, 22, 21, 0, 17, 19, 13, 11, 6, 20, 15, 23, 16, 2, 4, 9, 12, 1, 18, 10, 3, 24, 14, 8, 5 },
    .notches = { 17 },
    .numNotches = 1,
    .idx = 0,
};

// Alphabet: "VZBRGITYUPSDNHLXAWMJQOFECK"
// Notches: Z
static const enigma_rotor_t enigma_rotor_V = {
    .name = "V",
    .fwd_indices = { 21, 25, 1, 17, 6, 8, 19, 24, 20, 15, 18, 3, 13, 7, 11, 23, 0, 22, 12, 9, 16, 14, 5, 4, 2, 10 },
    .rev_indices = { 16, 2, 24, 11, 23, 22, 4, 13, 5, 19, 25, 14, 18, 12, 21, 9, 20, 3, 10, 6, 8, 0, 17, 15, 7, 1 },
    .notches = { 25 },
    .numNotches = 1,
    .idx = 0,
};

// Alphabet = "JPGVOUMFYQBENHZRDKASXLICTW"
// Notches: ZM
static const enigma_rotor_t enigma_rotor_VI = {
    .name = "VI",
    .fwd_indices = { 9, 15, 6, 21, 14, 20, 12, 5, 24, 16, 1, 4, 13, 7, 25, 17, 3, 10, 0, 18, 23, 11, 8, 2, 19, 22 },
    .rev_indices = { 18, 10, 23, 16, 11, 7, 2, 13, 22, 0, 17, 21, 6, 12, 4, 1, 9, 15, 19, 24, 5, 3, 25, 20, 8, 14 },
    .notches = { 25, 12 },
    .numNotches = 2,
    .idx = 0,
};

// Alphabet: "NZJHGRCXMYSWBOUFAIVLPEKQDT"
// Notches: ZM
static const enigma_rotor_t enigma_rotor_VII = {
    .name = "VII",
    .fwd_indices =  { 13, 25, 9, 7, 6, 17, 2, 23, 12, 24, 18, 22, 1, 14, 20, 5, 0, 8, 21, 11, 15, 4, 10, 16, 3, 19 },
    .rev_indices = { 16, 12, 6, 24, 21, 15, 4, 3, 17, 2, 22, 19, 8, 0, 13, 20, 23, 5, 10, 25, 14, 18, 11, 7, 9, 1 },
    .notches = { 25, 12 },
    .numNotches = 2,
    .idx = 0,
};

// Alphabet: "FKQHTLXOCBJSPDZRAMEWNIUYGV"
// Notches: ZM
static const enigma_rotor_t enigma_rotor_VIII = {
    .name = "VIII",
    .fwd_indices = { 5, 10, 16, 7, 19, 11, 23, 14, 2, 1, 9, 18, 15, 3, 25, 17, 0, 12, 4, 22, 13, 8, 20, 24, 6, 21 },
    .rev_indices = { 16, 9, 8, 13, 18, 0, 24, 3, 21, 10, 1, 5, 17, 20, 7, 12, 2, 15, 11, 4, 22, 25, 19, 6, 23, 14 },
    .notches = { 25, 12 },
    .numNotches = 2,
    .idx = 0,
};

static const enigma_rotor_t* enigma_rotors[] = {
    &enigma_rotor_I,
    &enigma_rotor_II,
    &enigma_rotor_III,
    &enigma_rotor_IV,
    &enigma_rotor_V,
    &enigma_rotor_VI,
    &enigma_rotor_VII,
    &enigma_rotor_VIII
};

#endif
