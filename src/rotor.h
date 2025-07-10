#ifndef ENIGMA_ROTOR_H
#define ENIGMA_ROTOR_H

typedef struct {
    const char *name;
    const char alphabet[26];
    const char *notches;
    const char turnover;
    int position;
} rotor_t;

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
    .turnover = 'E',
    .position = 0
};

rotor_t enigma_I_rotor_III = {
    .name = "III",
    .alphabet = "BDFHJLCPRTXVZNYEIWGAKMUSQO",
    .notches = "D",
    .turnover = 'V',
    .position = 0
};

rotor_t enigma_I_rotor_IV = {
    .name = "IV",
    .alphabet = "ESOVPZJAYQUIRHXLNFTGKDCMWB",
    .notches = "R",
    .turnover = 'J',
    .position = 0
};

rotor_t enigma_I_rotor_V = {
    .name = "V",
    .alphabet = "VZBRGITYUPSDNHLXAWMJQOFECK",
    .notches = "Z",
    .turnover = 'Z',
    .position = 0
};

#endif
