#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enigma.h"
#include "reflectors.h"
#include "rotors.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define VERBOSE_PRINT(fmt, ...)
#endif

#define ALPHA2IDX(c) ((c) - 'A')

static inline int index_of(const char*, char);
static inline int reflect(reflector_t*, int);
static inline void rotate(rotor_t *rotor);
static inline void rotate_rotors(enigma_t*);
static inline int rotor_pass_forward(rotor_t*, int);
static inline int rotor_pass_reverse(rotor_t*, int);
static inline char substitute(const char*, char);
static inline int to_alpha(int, int);
static inline int to_char_code(char);

/**
 * @brief Encode a character using the Enigma machine.
 *
 * This function takes an input character, processes it through the Enigma machine's rotors and reflector,
 * and returns the encoded character. It handles both uppercase and lowercase characters.
 *
 * Assumes the input character is an uppercase ASCII letter.
 *
 * @param enigma Pointer to the Enigma machine structure.
 * @param c The character to encode.
 * @return The encoded character.
 */
char enigma_encode(enigma_t* enigma, char c) {
    const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int idx = c - 'A';

    VERBOSE_PRINT("Keyboard Input: %c\n", c);

    rotate_rotors(enigma);

#ifdef VERBOSE
    VERBOSE_PRINT("%s", "Rotor Positions:");
    for (int i = 0; i < enigma->rotor_count; i++) {
        VERBOSE_PRINT(" %c", enigma->rotors[i].idx + 'A');
    }
    VERBOSE_PRINT("%s", "\n");
#endif

    // Plugboard
    c = substitute(enigma->plugboard, c);
    idx = ALPHA2IDX(c);
    VERBOSE_PRINT("Plugboard: %c (index %d)\n", c, idx);

    // Rotors
    for (int i = 0; i < enigma->rotor_count; i++) {
        idx = rotor_pass_forward(&enigma->rotors[i], idx);
    }

    // Reflector
    idx = enigma->reflector->indices[idx];
    VERBOSE_PRINT("Reflector %s (index %d): %c\n", enigma->reflector->name, idx, alphabet[idx]);

    // Rotors in reverse
    for (int i = enigma->rotor_count - 1; i >= 0; i--) {
        idx = rotor_pass_reverse(&enigma->rotors[i], idx);
        VERBOSE_PRINT("Rotor %s (index %d, offset %d): %c\n", enigma->rotors[i].alphabet, idx, enigma->rotors[i].idx, alphabet[idx]);
    }

    // Plugboard again
    c = substitute(enigma->plugboard, alphabet[idx]);
    VERBOSE_PRINT("Plugboard: %c\n", c);

    return c;
}

/**
 * @brief Encode a string using the Enigma machine.
 *
 * This function encodes a string of characters using the Enigma machine.
 * It processes each character through the machine and stores the result in the output string.
 * Assumes the input string is uppercase and null-terminated.
 *
 * @param enigma Pointer to the Enigma machine structure.
 * @param input The input string to encode.
 * @param output The output string to store the encoded result.
 * @param length The length of the input string (the output buffer should be at least the same length).
 */
void enigma_encode_string(enigma_t* enigma, const char* input, char* output, int length) {
    for (int i = 0; i < length; i++) {
        output[i] = enigma_encode(enigma, input[i]);
    }
    output[length] = '\0';
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
void enigma_init_rotors(enigma_t* enigma, const rotor_t* rotors, int count) {
    enigma->rotor_flag = 0;
    memcpy(enigma->rotors, rotors, count * sizeof(rotor_t));
    enigma->rotor_count = count;
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
void enigma_init_default_config(enigma_t* enigma) {
    enigma->reflector = &UKW_B;
    enigma->rotor_count = 3;
    enigma->rotors[2] = rotor_I;
    enigma->rotors[1] = rotor_II;
    enigma->rotors[0] = rotor_III;
    enigma->rotor_flag = 0;
    enigma->plugboard = NULL;
}

/**
 * @brief Find the index of a character in a string.
 *
 * This function searches for the first occurrence of a character in a string
 * and returns its index. Assumes character is present in the string.
 *
 * @param str The string to search in.
 * @param c The character to find.
 * @return The index of the character in the string, or -1 if not found.
 */
static __attribute__((always_inline)) inline int index_of(const char* str, char c) {
    const char* p = strchr(str, c);
    return (int)(p - str);
}

static __attribute__((always_inline)) inline void rotate(rotor_t *rotor) {
    rotor->idx++;
    if (rotor->idx == ALPHA_SIZE) {
        rotor->idx = 0;
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
static __attribute__((always_inline)) inline void rotate_rotors(enigma_t* enigma) {
    int turned = 0;
    for (int i = 0; i < enigma->rotors[1].numNotches; i++) {
        if (enigma->rotors[1].fwd_indices[enigma->rotors[1].idx] == enigma->rotors[1].notches[i]) {
            rotate(&enigma->rotors[1]);
            rotate(&enigma->rotors[2]);
            turned = 1;
            break;
        }
    }

    if (!turned) {

    for (int i = 0; i < enigma->rotors[0].numNotches; i++) {
        if (enigma->rotors[0].fwd_indices[enigma->rotors[0].idx] == enigma->rotors[0].notches[i]) {
            rotate(&enigma->rotors[1]);
            break;
        }
    }

    }
    rotate(&enigma->rotors[0]);
}

/**
 * @brief Pass through a rotor in the forward direction.
 *
 * @param rotor Pointer to the `rotor_t`.
 * @param idx The index of the character in the alphabet.
 *
 * @return The index of the character after passing through the rotor.
 */
static __attribute__((always_inline)) inline int rotor_pass_forward(rotor_t* rotor, int idx) {
    idx = idx + rotor->idx;
    if (idx >= ALPHA_SIZE) {
        idx -= ALPHA_SIZE;
    }

    idx = rotor->fwd_indices[idx];

    idx = (ALPHA_SIZE + idx - rotor->idx);
    if (idx >= ALPHA_SIZE) {
        idx -= ALPHA_SIZE;
    }
    return idx;
}

/**
 * @brief Pass through a rotor in the reverse direction.
 *
 * @param rotor Pointer to the `rotor_t`.
 * @param idx The index of the character in the alphabet.
 *
 * @return The index of the character after passing through the rotor.
 */
static __attribute__((always_inline)) inline int rotor_pass_reverse(rotor_t* rotor, int idx) {
    idx += rotor->idx;
    if (idx >= ALPHA_SIZE) {
        idx -= ALPHA_SIZE;
    }

    idx = rotor->rev_indices[idx];

    idx = (ALPHA_SIZE + idx - rotor->idx);
    if (idx >= ALPHA_SIZE) {
        idx -= ALPHA_SIZE;
    }
    return idx;
}

/**
 * @brief Substitute characters based on the plugboard configuration.
 *
 * This function takes in the plugboard configuration (an even-length string
 * representing pairs of characters to swap), and the character to be substituted.
 * The character must be uppercase.
 *
 * Assumes an even-length string.
 *
 * @param plugboard The plugboard configuration string.
 * @param c The character to be substituted.
 * @return The substituted character based on the plugboard configuration.
 */
static __attribute__((always_inline)) inline char substitute(const char* plugboard, char c) {
    if (!plugboard) return c;

    for (const char *p = plugboard; p[0]; p += 2) {
        if (p[0] == c) return p[1];
        if (p[1] == c) return p[0];
    }

    return c;
}
