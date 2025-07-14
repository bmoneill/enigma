#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "enigma.h"

#define VERBOSE_PRINT(fmt, ...) if (verbose) fprintf(stderr, fmt, __VA_ARGS__)

int verbose = 1;

static int get_index(char c);
static char reflect(enigma_t *, char);
static void rotate(rotor_t *, int);
static void rotate_rotors(enigma_t *);
static char rotor_pass(enigma_t *, int, int, char);
static char substitute(const char *, char);

char encode(enigma_t *enigma, char input) {
    VERBOSE_PRINT("Keyboard Input: %c\n", input);

    rotate_rotors(enigma);
    if (verbose) {
        VERBOSE_PRINT("%s", "Rotor Positions:");
        for (int i = 0; i < enigma->rotor_count; i++) {
            VERBOSE_PRINT(" %c", enigma->rotors[i].alphabet[0]);
        }
        VERBOSE_PRINT("%s", "\n");
    }

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

static int get_index(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a';
    }
    return -1;
}

static void init_rotors(enigma_t *enigma, rotor_t *rotors, int count) {
    enigma->rotors = malloc(count * sizeof(rotor_t));
    memcpy(enigma->rotors, rotors, count * sizeof(rotor_t));

    for (int i = 0; i < count; i++) {
        enigma->rotors[i].alphabet = malloc(27 * sizeof(char));
        memcpy(enigma->rotors[i].alphabet, rotors[i].alphabet, 27 * sizeof(char));
    }
    enigma->rotor_count = count;
}

static void rotate(rotor_t *rotor, int count) {
    rotor->idx += count;

    if (rotor->idx >= 26) {
        rotor->idx -= 26;
    }

    for (int i = 0; i < count; i++) {
        char first = rotor->alphabet[0];
        for (int j = 0; j < 25; j++) {
            rotor->alphabet[j] = rotor->alphabet[j + 1];
        }
        rotor->alphabet[25] = first;
    }
}

static void rotate_rotors(enigma_t *enigma) {
    rotate(&enigma->rotors[0], 1);

    if (enigma->rotor_flag) {
        rotate(&enigma->rotors[1], 1);
        rotate(&enigma->rotors[2], 1);
    }

    // TODO fix for multiple notches
    if (enigma->rotors[0].alphabet[0] == enigma->rotors[0].notches[0]) {
        rotate(&enigma->rotors[1], 1);
    }
}

static char rotor_pass(enigma_t *enigma, int rotorIdx, int direction, char input) {
    rotor_t *rotor = &enigma->rotors[rotorIdx];
    int index = get_index(input);


    if (isupper(input)) {
        return rotor->alphabet[index];
    }

    return tolower(rotor->alphabet[index]);
}

static char reflect(enigma_t *enigma, char input) {
    if (!enigma->reflector) {
        return input;
    }

    int index = get_index(input);

    if (isupper(input)) {
        return enigma->reflector->alphabet[index];
    }

    return tolower(enigma->reflector->alphabet[index]);
}

static char substitute(const char *plugboard, char input) {
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
