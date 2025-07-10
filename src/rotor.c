#include "rotor.h"

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
