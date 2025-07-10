#include "plugboard.h"

char plugboard_substitute(plugboard_t *plugboard, char input) {
    for (int i = 0; plugboard->wiring[i] != '\0'; i += 2) {
        if (plugboard->wiring[i] == input) {
            return plugboard->wiring[i + 1];
        } else if (plugboard->wiring[i + 1] == input) {
            return plugboard->wiring[i];
        }
    }
    return input;
}
