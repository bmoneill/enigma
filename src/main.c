#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "rotors.h"
#include "reflectors.h"

int main(int argc, char *argv[]) {
    enigma_t enigma;
    enigma.name = "Enigma Machine";
    enigma.rotor_count = 3;
    enigma.rotors = malloc(enigma.rotor_count * sizeof(rotor_t));
    enigma.reflector = malloc(sizeof(reflector_t));
    memcpy(enigma.reflector, &enigma_1_UKW_B, sizeof(reflector_t));

    enigma.plugboard = ""; // Example plugboard configuration
    init_rotors(&enigma, (rotor_t[]) { enigma_I_rotor_I, enigma_I_rotor_II, enigma_I_rotor_III }, enigma.rotor_count);

    int c;
    while ((c = fgetc(stdin)) != EOF) {
        // Read input from stdin until EOF
        if (c == '\n') {
            fputc('\n', stdout);
            continue;
        }
        char output = encode(&enigma, c);
        fputc(output, stdout);
    }
    printf("\n");

    free(enigma.rotors);
    free(enigma.reflector);
    return 0;
}
