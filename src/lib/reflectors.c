#include "reflectors.h"

// Alphabet: "EJMZALYXVBWFCRQUONTSPIKHGD"
reflector_t UKW_A = {
    .name = "A",
    .indices = {4, 9, 12, 25, 0, 1, 11, 23, 14, 19, 18, 17, 16, 15, 13, 10, 3, 2, 5, 6, 7, 8, 20, 21, 22},
};

// Alphabet: "YRUHQSLDPXNGOKMIEBFZCWVJAT"
reflector_t UKW_B = {
    .name = "B",
    .indices = { 24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14, 10, 12, 8, 4, 1, 5, 25, 2, 22, 21, 9, 0, 19},
};

// Alphabet: "FVPJIAOYEDRZXWGCTKUQSBNMHL"
reflector_t UKW_C = {
    .name = "C",
    .indices = { 5, 21, 15, 9, 8, 0, 14, 24, 4, 3, 17, 25, 23, 22, 6, 2, 19, 10, 20, 16, 18, 1, 13, 12, 7, 11},
};

reflector_t* enigma_reflectors[] = {
    &UKW_A,
    &UKW_B,
    &UKW_C,
};
