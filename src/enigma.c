#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"

char substitute(const char *, char);
char rotor_pass(enigma_t *, int, int, char);

char encode(enigma_t *enigma, char input) {
    // Convert input to uppercase
    char output = toupper(input);

    // Pass through the plugboard first
    output = substitute(enigma->plugboard, output);

    // Pass through the rotors
    for (int i = 0; i < enigma->rotor_count; i++) {
        output = rotor_pass(enigma, i, 1, output);
    }

    // Pass through the reflector
    if (enigma->reflector) {
        int index = output - 'A';
        output = enigma->reflector->alphabet[index];
    }

    // Pass back through the rotors in reverse order
    for (int i = enigma->rotor_count - 1; i >= 0; i--) {
        output = rotor_pass(enigma, i, -1, output);
    }
    return output;
}

char rotor_pass(enigma_t *enigma, int rotorIdx, int direction, char input) {
    rotor_t *rotor = &enigma->rotors[rotorIdx];
    int offset = rotor->position;
    int index = (input - 'A' + offset) % 26;

    if (rotor->notches) {
        // Rotate next rotor if at a notch position
        if (rotor->notches[0] == rotor->alphabet[index]) {
            if (rotorIdx + direction < enigma->rotor_count && rotorIdx + direction > 0) {
                enigma->rotors[rotorIdx + direction].position = (enigma->rotors[rotorIdx + direction].position + 1) % 26;
            }
        }
    }

    return rotor->alphabet[index];
}

char substitute(const char *plugboard, char input) {
    if (!plugboard) return input;

    for (int i = 0; plugboard[i] != '\0'; i += 2) {
        if (plugboard[i] == input) {
            return plugboard[i + 1];
        } else if (plugboard[i + 1] == input) {
            return plugboard[i];
        }
    }
    return input;
}
