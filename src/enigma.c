#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "enigma.h"

#define VERBOSE_PRINT(fmt, ...) if (verbose) fprintf(stderr, fmt, __VA_ARGS__)

int verbose = 1;

int get_index(char c);
char reflect(enigma_t *, char);
void rotate(rotor_t *, int);
char rotor_pass(enigma_t *, int, int, char);
char substitute(const char *, char);

char encode(enigma_t *enigma, char input) {
    char output = substitute(enigma->plugboard, input);
    VERBOSE_PRINT("Plugboard: %c -> %c\n", input, output);

    for (int i = 0; i < enigma->rotor_count; i++) {
        input = output;
        output = rotor_pass(enigma, i, 1, output);
        VERBOSE_PRINT("Rotor %d: %c -> %c\n", i + 1, input, output);
    }

    input = output;
    output = reflect(enigma, output);
    VERBOSE_PRINT("Reflector %s: %c -> %c\n", enigma->reflector->name, input, output);

    for (int i = enigma->rotor_count - 1; i >= 0; i--) {
        input = output;
        output = rotor_pass(enigma, i, -1, output);
        VERBOSE_PRINT("Rotor %d: %c -> %c\n", i + 1, input, output);
    }

    input = output;
    output = substitute(enigma->plugboard, output);
    VERBOSE_PRINT("Plugboard: %c -> %c\n", input, output);

    return output;
}

int get_index(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a';
    }
    return -1;
}

void init_rotors(enigma_t *enigma, rotor_t *rotors, int count) {
    enigma->rotors = malloc(count * sizeof(rotor_t));
    memcpy(enigma->rotors, rotors, count * sizeof(rotor_t));

    for (int i = 0; i < count; i++) {
        enigma->rotors[i].alphabet = malloc(27 * sizeof(char));
        memcpy(enigma->rotors[i].alphabet, rotors[i].alphabet, 27 * sizeof(char));
    }
    enigma->rotor_count = count;
}

void rotate(rotor_t *rotor, int count) {
    for (int i = 0; i < count; i++) {
        char first = rotor->alphabet[0];
        for (int j = 0; j < 25; j++) {
            rotor->alphabet[j] = rotor->alphabet[j + 1];
        }
        rotor->alphabet[25] = first;
    }
}

char rotor_pass(enigma_t *enigma, int rotorIdx, int direction, char input) {
    rotor_t *rotor = &enigma->rotors[rotorIdx];
    int index = get_index(input);

    if (rotorIdx == 0 && direction == 1) {
        VERBOSE_PRINT("Rotating rotor %d\n", rotorIdx + 1);
        rotate(rotor, direction);
    } else if (rotorIdx > 0 && enigma->rotors[rotorIdx - 1].alphabet[0] == rotor->alphabet[index]) {
        // Rotate the previous rotor if the current rotor is at a notch position
        VERBOSE_PRINT("Rotating rotor %d\n", rotorIdx);
        rotate(&enigma->rotors[rotorIdx - 1], direction);
    }

    if (isupper(input)) {
        return rotor->alphabet[index];
    }

    return tolower(rotor->alphabet[index]);
}

char reflect(enigma_t *enigma, char input) {
    if (!enigma->reflector) return input;

    int index = get_index(input);

    if (isupper(input)) {
        return enigma->reflector->alphabet[index];
    }

    return tolower(enigma->reflector->alphabet[index]);
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
