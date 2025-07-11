#include <stdio.h>
#include "enigma.h"

int main(int argc, char *argv[]) {
    enigma_t enigma;
    enigma.name = "Enigma Machine";
    enigma.rotor_count = 3;
    enigma.rotors = malloc(enigma.rotor_count * sizeof(rotor_t));
    enigma.reflector = &enigma_1_UKW_A; // Example reflector
    enigma.plugboard = "AB CD EF"; // Example plugboard configuration
    memcpy(enigma.rotors, (rotor_t[]) {
    }, enigma.rotor_count * sizeof(rotor_t));
    return 0;
}
