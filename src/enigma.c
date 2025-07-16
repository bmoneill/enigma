#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "enigma.h"

#define VERBOSE_PRINT(fmt, ...) if (verbose) fprintf(stderr, fmt, __VA_ARGS__)
#define ALPHA_SIZE 26

const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int verbose = 1;

static int index_of(const char *, char);
static char reflect(enigma_t *, int, int);
static void rotate(rotor_t *, int);
static void rotate_rotors(enigma_t *);
static int rotor_pass(enigma_t *, int, int, int);
static char substitute(const char *, char, int);
static int to_alpha(int, int);
static int to_char_code(char);

char encode(enigma_t *enigma, char input) {
    char output = input;
    int upper = isupper(input);
    int idx = to_char_code(input);

    VERBOSE_PRINT("Keyboard Input: %c\n", input);

    rotate_rotors(enigma);

    if (verbose) {
        VERBOSE_PRINT("%s", "Rotor Positions:");
        for (int i = 0; i < enigma->rotor_count; i++) {
            VERBOSE_PRINT(" %c", enigma->rotors[i].idx + 'A');
        }
        VERBOSE_PRINT("%s", "\n");
    }

    // Plugboard
    input = substitute(enigma->plugboard, input, upper);
    idx = to_char_code(input);
    VERBOSE_PRINT("Plugboard: %c\n", input);

    // Rotors
    for (int i = 0; i < enigma->rotor_count; i++) {
        idx = (idx + enigma->rotors[i].idx) % ALPHA_SIZE;
        idx = to_char_code(enigma->rotors[i].alphabet[idx]);
        idx = (ALPHA_SIZE + idx - enigma->rotors[i].idx) % ALPHA_SIZE;
        VERBOSE_PRINT("Rotor %d (index %d): %c\n", i + 1, idx, enigma->rotors[i].alphabet[idx]);
    }

    // Reflector
    idx = to_char_code(enigma->reflector->alphabet[idx]);
    VERBOSE_PRINT("Reflector %s (index %d): %c\n", enigma->reflector->name, idx, alphabet[idx]);

    // Rotors in reverse
    for (int i = enigma->rotor_count - 1; i >= 0; i--) {
        idx = (idx + enigma->rotors[i].idx) % ALPHA_SIZE;
        idx = to_char_code(enigma->rotors[i].alphabet[idx]);
        idx = (ALPHA_SIZE + idx - enigma->rotors[i].idx) % ALPHA_SIZE;

        VERBOSE_PRINT("Rotor %d (index %d): %c\n", i + 1, idx, enigma->rotors[i].alphabet[idx]);
    }

    // Plugboard again
    output = substitute(enigma->plugboard, enigma->rotors[0].alphabet[idx], upper);
    VERBOSE_PRINT("Plugboard: %c\n", output);

    return output;
}

void init_rotors(enigma_t *enigma, const rotor_t *rotors, int count) {
    enigma->rotor_flag = 0;
    enigma->rotors = malloc(count * sizeof(rotor_t));
    memcpy(enigma->rotors, rotors, count * sizeof(rotor_t));
    enigma->rotor_count = count;
}

static int index_of(const char *str, char c) {
    const char *p = strchr(str, c);
    return p ? (int)(p - str) : -1;
}

static char reflect(enigma_t *enigma, int idx, int upper) {
    reflector_t *reflector = enigma->reflector;

    if (!reflector) {
        printf("Error: Reflector not set.\n");
        return idx;
    }

    return upper ? reflector->alphabet[idx] : tolower(reflector->alphabet[idx]);
}


static void rotate(rotor_t *rotor, int count) {
    rotor->idx += count;

    if (rotor->idx >= ALPHA_SIZE) {
        rotor->idx -= ALPHA_SIZE;
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

static char substitute(const char *plugboard, char c, int upper) {
    printf("%c\n", c);
    if (!plugboard) return c;

    c = toupper(c);

    for (int i = 0; plugboard[i] != '\0'; i += 2) {
        printf("passed\n");
        if (plugboard[i] == c) {
            return upper ? plugboard[i + 1] : tolower(plugboard[i + 1]);
        } else if (plugboard[i + 1] == c) {
            return upper ? plugboard[i] : tolower(plugboard[i]);
        }
    }

    return c;
}

static int to_char_code(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a';
    }
    return -1;
}

static int to_alpha(int c, int upper) {
    return c + (upper ? 'A' : 'a');
}
