#ifndef ENIGMA_H
#define ENIGMA_H

typedef struct {
    const char *name;
    const char *alphabet;
} reflector_t;

typedef struct {
    const char *name;
    const char *alphabet;
    const char *notches;
    int position;
} rotor_t;

typedef struct {
    char *name;
    rotor_t *rotors;
    int rotor_count;
    reflector_t *reflector;
    const char *plugboard;
} enigma_t;

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

rotor_t enigma_I_rotor_I = {
    .name = "I",
    .alphabet = "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
    .notches = "Y",
    .turnover = 'Q',
    .position = 0
};

rotor_t enigma_I_rotor_II = {
    .name = "II",
    .alphabet = "AJDKSIRUXBLHWTMCQGZNPYFVOE",
    .notches = "M",
    .position = 0
};

rotor_t enigma_I_rotor_III = {
    .name = "III",
    .alphabet = "BDFHJLCPRTXVZNYEIWGAKMUSQO",
    .notches = "D",
    .position = 0
};

rotor_t enigma_I_rotor_IV = {
    .name = "IV",
    .alphabet = "ESOVPZJAYQUIRHXLNFTGKDCMWB",
    .notches = "R",
    .position = 0
};

rotor_t enigma_I_rotor_V = {
    .name = "V",
    .alphabet = "VZBRGITYUPSDNHLXAWMJQOFECK",
    .notches = "Z",
    .position = 0
};


char encode(enigma_t*, char);
char rotor_pass(enigma_t *enigma, int rotorIdx, int direction, char input);

#endif
