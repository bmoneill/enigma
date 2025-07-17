#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define VERBOSE_PRINT(fmt, ...)
#endif

const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int index_of(const char *, char);
static int reflect(reflector_t *, int);
static void rotate(rotor_t *, int);
static void rotate_rotors(enigma_t *);
static int rotor_pass(rotor_t *, int, int);
static char substitute(const char *, char, int);
static int to_alpha(int, int);
static int to_char_code(char);

/**
 * @brief Encode a character using the Enigma machine.
 *
 * This function takes an input character, processes it through the Enigma machine's rotors and reflector,
 * and returns the encoded character. It handles both uppercase and lowercase characters.
 *
 * @param enigma Pointer to the Enigma machine structure.
 * @param input The character to encode.
 * @return The encoded character.
 */
char encode(enigma_t *enigma, char input) {
    char output = input;
    int upper = isupper(input);
    int idx = to_char_code(input);

    VERBOSE_PRINT("Keyboard Input: %c\n", input);

    rotate_rotors(enigma);

    #ifdef VERBOSE
        VERBOSE_PRINT("%s", "Rotor Positions:");
        for (int i = 0; i < enigma->rotor_count; i++) {
            VERBOSE_PRINT(" %c", enigma->rotors[i].idx + 'A');
        }
        VERBOSE_PRINT("%s", "\n");
    #endif

    // Plugboard
    input = substitute(enigma->plugboard, input, upper);
    idx = to_char_code(input);
    VERBOSE_PRINT("Plugboard: %c (index %d)\n", input, idx);

    // Rotors
    for (int i = 0; i < enigma->rotor_count; i++) {
        idx = rotor_pass(&enigma->rotors[i], idx, 1);
        VERBOSE_PRINT("Rotor %s (index %d): %c\n", enigma->rotors[i].alphabet, idx, alphabet[idx]);
    }

    // Reflector
    output = reflect(enigma->reflector, idx);
    idx = to_char_code(output);
    VERBOSE_PRINT("Reflector %s (index %d): %c\n", enigma->reflector->name, idx, alphabet[idx]);

    // Rotors in reverse
    for (int i = enigma->rotor_count - 1; i >= 0; i--) {
        idx = rotor_pass(&enigma->rotors[i], idx, -1);
        VERBOSE_PRINT("Rotor %s (index %d, offset %d): %c\n", enigma->rotors[i].alphabet, idx, enigma->rotors[i].idx, alphabet[idx]);
    }

    // Plugboard again
    output = substitute(enigma->plugboard, alphabet[idx], upper);
    VERBOSE_PRINT("Plugboard: %c\n", output);

    return output;
}

/**
 * @brief Initialize the rotors of the Enigma machine.
 *
 * This populates the `rotors` array in the specified `enigma_t` by copying the provided
 * array of rotors. It also sets the rotor flag and count.
 *
 * @param enigma Pointer to the `enigma_t`.
 * @param rotors Array of `rotor_t`s to copy to the `enigma_t`.
 * @param count Number of rotors to copy.
 */
void init_rotors(enigma_t *enigma, const rotor_t *rotors, int count) {
    enigma->rotor_flag = 0;
    enigma->rotors = malloc(count * sizeof(rotor_t));
    memcpy(enigma->rotors, rotors, count * sizeof(rotor_t));
    enigma->rotor_count = count;
}

/**
 * @brief Find the index of a character in a string.
 *
 * This function searches for the first occurrence of a character in a string
 * and returns its index. If the character is not found, it returns -1.
 *
 * @param str The string to search in.
 * @param c The character to find.
 * @return The index of the character in the string, or -1 if not found.
 */
static int index_of(const char *str, char c) {
    const char *p = strchr(str, c);
    return p ? (int)(p - str) : -1;
}

/**
 * @brief Load the default configuration for the Enigma machine.
 *
 * Default config is as follows:
 * - Reflector: UKW-B
 * - Rotors: I, II, III
 * - Rotor positions: A, A, A
 * - Empty plugboard
 *
 * @param enigma Pointer to the `enigma_t` to be initialized.
 */
void init_default_enigma(enigma_t *enigma) {
    enigma->reflector = malloc(sizeof(reflector_t));
    memcpy(enigma->reflector, &UKW_B, sizeof(reflector_t));
    enigma->rotor_count = 3;
    enigma->rotors = malloc(enigma->rotor_count * sizeof(rotor_t));
    enigma->rotors[2] = arotor_I;
    enigma->rotors[1] = rotor_II;
    enigma->rotors[0] = rotor_III;
    enigma->rotor_flag = 0;
    enigma->plugboard = NULL;
}


/**
 * @brief Pass through the reflector and return the reflected character.
 *
 * @param reflector Pointer to the reflector structure.
 * @param idx The index where the ciphertext left the last rotor.
 * @param upper A flag indicating if the character is uppercase (1) or lowercase (0).
 * @return The reflected character based on the reflector's alphabet.
 */
static int reflect(reflector_t *reflector, int idx) {
    if (!reflector) {
        fprintf(stderr, "Warning: Reflector not set.\n");
        return idx;
    }

    return reflector->alphabet[idx];
}

/**
 * @brief Rotate the specified rotor by the specified count.
 *
 * This function rotates the rotor's index by the given count, wrapping around
 * if it exceeds the size of the alphabet.
 *
 * @param rotor Pointer to the rotor structure.
 * @param count The number of positions to rotate.
 */
static void rotate(rotor_t *rotor, int count) {
    rotor->idx += count;

    if (rotor->idx >= ALPHA_SIZE) {
        rotor->idx -= ALPHA_SIZE;
    }
}

/**
 * @brief Rotate the rotors of the Enigma machine.
 *
 * This function rotates the first rotor by one position and checks if the
 * next rotors need to be rotated based on the notches.
 *
 * @param enigma Pointer to the Enigma machine structure.
 */
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

/**
 * @brief Pass through a rotor and return the index of the character.
 *
 * This function passes through a rotor, given the index that the wiring left
 * the last rotor. It returns the index of the character in the rotor's alphabet
 * (the index which it left this rotor).
 *
 * @param rotor Pointer to the `rotor_t`.
 * @param idx The index of the character in the alphabet.
 * @param direction The direction of the pass (1 for forward, -1 for reverse)
 * @return The index of the character after passing through the rotor.
 */
static int rotor_pass(rotor_t *rotor, int idx, int direction) {
    idx = (idx + rotor->idx) % ALPHA_SIZE;
    if (direction == 1) {
        idx = index_of(alphabet, rotor->alphabet[idx]);
    } else {
        idx = index_of(rotor->alphabet, alphabet[idx]);
    }
    return (ALPHA_SIZE + idx - rotor->idx) % ALPHA_SIZE;
}

/**
 * @brief Substitute characters based on the plugboard configuration.
 *
 * This function takes in the plugboard configuration (an even-length string
 * representing pairs of characters to swap), and the character to be substituted.
 * The character can be uppercase or lowercase.
 *
 * @param plugboard The plugboard configuration string.
 * @param c The character to be substituted.
 * @param upper A flag indicating if the character is uppercase (1) or lowercase (0)
 * @return The substituted character based on the plugboard configuration.
 */
static char substitute(const char *plugboard, char c, int upper) {
    if (!plugboard) return c;

    c = toupper(c);

    for (int i = 0; plugboard[i] != '\0'; i += 2) {
        if (plugboard[i] == c) {
            return upper ? plugboard[i + 1] : tolower(plugboard[i + 1]);
        } else if (plugboard[i + 1] == c) {
            return upper ? plugboard[i] : tolower(plugboard[i]);
        }
    }

    return c;
}

/**
 * @brief Convert a character to its corresponding index in the alphabet.
 * @param c character to convert
 * @return The index of the character in the alphabet (0 for 'A' or 'a'...)
 */
static int to_char_code(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a';
    }
    return -1;
}

/**
 * @brief Convert an index to its corresponding character in the alphabet (with correct casing).
 * @param c The index to convert.
 * @param upper A flag indicating if the character should be uppercase (1) or lowercase (0).
 * @return The character corresponding to the index in the alphabet.
 */
static int to_alpha(int c, int upper) {
    return c + (upper ? 'A' : 'a');
}
