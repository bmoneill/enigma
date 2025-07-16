#include "reflectors.h"

const reflector_t UKW_A = {
    .name = "A",
    .alphabet = "EJMZALYXVBWFCRQUONTSPIKHGD",
};

const reflector_t UKW_B = {
    .name = "B",
    .alphabet = "YRUHQSLDPXNGOKMIEBFZCWVJAT",
};

const reflector_t UKW_C = {
    .name = "C",
    .alphabet = "FVPJIAOYEDRZXWGCTKUQSBNMHL",
};

const reflector_t *enigma_reflectors[] = {
    &UKW_A,
    &UKW_B,
    &UKW_C,
};
