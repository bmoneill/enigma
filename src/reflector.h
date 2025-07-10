#ifndef REFLECTOR_H
#define REFLECTOR_H

typedef struct {
    const char *name;
    const char alphabet[26];
} reflector_t;

reflector_t enigma_1_UKW_A = {
    .name = "UKW-A",
    .alphabet = "EJMZALYXVBWFCRQUONTSPIKHGD",
};

reflector_t enigma_1_UKW_B = {
    .name = "UKW-B",
    .alphabet = "YRUHQSLDPXNGOKMIEBFZCWVJAT",
};

reflector_t enigma_1_UKW_C = {
    .name = "UKW-C",
    .alphabet = "FVPJIAOYEDRZXWGCTKUQSBNMHL",
};

#endif