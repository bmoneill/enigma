#include "reflectors.h"

reflector_t UKW_A = {
    .name = "A",
    .alphabet = "EJMZALYXVBWFCRQUONTSPIKHGD",
};

reflector_t UKW_B = {
    .name = "B",
    .alphabet = "YRUHQSLDPXNGOKMIEBFZCWVJAT",
};

reflector_t UKW_C = {
    .name = "C",
    .alphabet = "FVPJIAOYEDRZXWGCTKUQSBNMHL",
};

reflector_t* enigma_reflectors[] = {
    &UKW_A,
    &UKW_B,
    &UKW_C,
};
