#ifndef ENIGMA_ROTORS_H
#define ENIGMA_ROTORS_H

#include "enigma.h"

/* I/M3/M4 ROTORS */
const rotor_t enigma_I_rotor_I = {
    .name = "I",
    .alphabet = "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
    .notches = "Q",
    .ring = 0,
    .idx = 0,
};

const rotor_t enigma_I_rotor_II = {
    .name = "II",
    .alphabet = "AJDKSIRUXBLHWTMCQGZNPYFVOE",
    .notches = "E",
    .ring = 0,
    .idx = 0,
};

const rotor_t enigma_I_rotor_III = {
    .name = "III",
    .alphabet = "BDFHJLCPRTXVZNYEIWGAKMUSQO",
    .notches = "V",
    .ring = 0,
    .idx = 0,
};

const rotor_t enigma_I_rotor_IV = {
    .name = "IV",
    .alphabet = "ESOVPZJAYQUIRHXLNFTGKDCMWB",
    .notches = "R",
    .ring = 0,
    .idx = 0,
};

const rotor_t enigma_I_rotor_V = {
    .name = "V",
    .alphabet = "VZBRGITYUPSDNHLXAWMJQOFECK",
    .notches = "Z",
    .ring = 0,
    .idx = 0,
};

#endif
