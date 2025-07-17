#include "rotors.h"

const rotor_t rotor_I = {
    .name = "I",
    .alphabet = "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
    .notches = "Q",
    .idx = 0,
};

const rotor_t rotor_II = {
    .name = "II",
    .alphabet = "AJDKSIRUXBLHWTMCQGZNPYFVOE",
    .notches = "E",
    .idx = 0,
};

const rotor_t rotor_III = {
    .name = "III",
    .alphabet = "BDFHJLCPRTXVZNYEIWGAKMUSQO",
    .notches = "V",
    .idx = 0,
};

const rotor_t rotor_IV = {
    .name = "IV",
    .alphabet = "ESOVPZJAYQUIRHXLNFTGKDCMWB",
    .notches = "R",
    .idx = 0,
};

const rotor_t rotor_V = {
    .name = "V",
    .alphabet = "VZBRGITYUPSDNHLXAWMJQOFECK",
    .notches = "Z",
    .idx = 0,
};

const rotor_t rotor_VI = {
    .name = "VI",
    .alphabet = "JPGVOUMFYQBENHZRDKASXLICTW",
    .notches = "ZM",
    .idx = 0,
};

const rotor_t rotor_VII = {
    .name = "VII",
    .alphabet = "NZJHGRCXMYSWBOUFAIVLPEKQDT",
    .notches = "ZM",
    .idx = 0,
};

const rotor_t rotor_VIII = {
    .name = "VIII",
    .alphabet = "FKQHTLXOCBJSPDZRAMEWNIUYGV",
    .notches = "ZM",
    .idx = 0,
};

const rotor_t* enigma_rotors[] = {
    &rotor_I,
    &rotor_II,
    &rotor_III,
    &rotor_IV,
    &rotor_V,
    &rotor_VI,
    &rotor_VII,
    &rotor_VIII
};
