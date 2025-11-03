/**
 * @file enigma/enigma.c
 *
 * This file implements the core functionality of the Enigma machine,
 * including encoding characters and strings, initializing rotors,
 * and setting up default and random configurations.
 */
#include "enigma.h"

#include "common.h"
#include "enigma/rotor.h"
#include "io.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ALPHA2IDX(c) ((c) - 'A')

static ENIGMA_ALWAYS_INLINE void rotate(int*);
static ENIGMA_ALWAYS_INLINE void rotate_rotors(enigma_t*);
static ENIGMA_ALWAYS_INLINE int  rotor_pass_forward(const enigma_rotor_t*, int, int);
static ENIGMA_ALWAYS_INLINE int  rotor_pass_reverse(const enigma_rotor_t*, int, int);
static ENIGMA_ALWAYS_INLINE char substitute(const char*, char);

/**
 * @brief Encode a character using the Enigma machine.
 *
 * This function takes an input character, processes it through the Enigma
 * machine's rotors and reflector, and returns the encoded character. It handles
 * both uppercase and lowercase characters.
 *
 * Assumes the input character is an uppercase ASCII letter.
 *
 * @param enigma Pointer to the Enigma machine structure.
 * @param c The character to encode.
 * @return The encoded character.
 */
EMSCRIPTEN_KEEPALIVE char enigma_encode(enigma_t* enigma, int c) {
    if (!enigma || c < 'A' || c > 'Z') {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    static const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    c -= 'A';

    rotate_rotors(enigma);

    // Plugboard
    c = substitute(enigma->plugboard, c + 'A') - 'A';

    // Rotors
    for (int i = 0; i < enigma->rotor_count; i++) {
        c = rotor_pass_forward(enigma->rotors[i], enigma->rotor_indices[i], c);
    }

    // Reflector
    c = enigma->reflector->indices[c];

    // Rotors in reverse
    for (int i = enigma->rotor_count - 1; i >= 0; i--) {
        c = rotor_pass_reverse(enigma->rotors[i], enigma->rotor_indices[i], c);
    }

    // Plugboard again
    c = substitute(enigma->plugboard, alphabet[c]);

    return c;
}

/**
 * @brief Encode a string using the Enigma machine.
 *
 * This function encodes a string of characters using the Enigma machine.
 * It processes each character through the machine and stores the result in the
 * output string. Assumes the input string is uppercase and null-terminated.
 *
 * @param enigma Pointer to the Enigma machine structure.
 * @param input The input string to encode.
 * @param output The output string to store the encoded result.
 * @param length The length of the input string (the output buffer should be at
 * least the same length).
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_encode_string(enigma_t* enigma, const char* input, char* output, int length) {
    if (!enigma || !input || !output || length <= 0) {
        return ENIGMA_FAILURE;
    }

    for (int i = 0; i < length; i++) {
        output[i] = enigma_encode(enigma, input[i]);
    }
    output[length] = '\0';

    return ENIGMA_SUCCESS;
}

/**
 * @brief Initialize the rotors of the Enigma machine.
 *
 * This populates the `rotors` array in the specified `enigma_t` by copying the
 * provided array of rotors. It also sets the rotor flag and count.
 *
 * @param enigma Pointer to the `enigma_t`.
 * @param rotors Array of `rotor_t`s to copy to the `enigma_t`.
 * @param count Number of rotors to copy.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_init_rotors(enigma_t* enigma, const enigma_rotor_t* rotors, int count) {
    if (!enigma || !rotors || count <= 0) {
        return ENIGMA_FAILURE;
    }
    enigma->rotor_flag = 0;
    for (int i = 0; i < count; i++) {
        enigma->rotors[i] = &rotors[i];
    }
    enigma->rotor_count = count;
    return ENIGMA_SUCCESS;
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
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_init_default_config(enigma_t* enigma) {
    if (!enigma) {
        return ENIGMA_FAILURE;
    }

    enigma->reflector        = &enigma_UKW_B;
    enigma->rotor_count      = 3;
    enigma->rotors[2]        = &enigma_rotor_I;
    enigma->rotors[1]        = &enigma_rotor_II;
    enigma->rotors[0]        = &enigma_rotor_III;
    enigma->rotor_indices[0] = 0;
    enigma->rotor_indices[1] = 0;
    enigma->rotor_indices[2] = 0;
    enigma->rotor_flag       = 0;
    memset(enigma->plugboard, 0, 27);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Load a random configuration for the Enigma machine.
 *
 * This function initializes the Enigma machine with a random configuration.
 *
 * @param enigma Pointer to the `enigma_t` to be initialized.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_init_random_config(enigma_t* enigma) {
    if (!enigma) {
        return ENIGMA_FAILURE;
    }
    srand(time(NULL));
    bool unique = false;
    char c      = '\0';

    if (rand() % 2) {
        enigma->rotor_count = 3;
    } else {
        enigma->rotor_count = 4;
    }

    for (int i = 0; i < enigma->rotor_count; i++) {
        const enigma_rotor_t* candidate = NULL;
        while (!unique) {
            unique    = 1;
            candidate = enigma_rotors[rand() % ENIGMA_ROTOR_COUNT];
            for (int j = 0; j < i; j++) {
                if (!strcmp(enigma->rotors[j]->name, candidate->name)) {
                    unique = 0;
                    break;
                }
            }
        }
        enigma->rotors[i]        = candidate;
        enigma->rotor_indices[i] = rand() % ENIGMA_ALPHA_SIZE;
        unique                   = 0;
    }

    enigma->reflector = enigma_reflectors[rand() % ENIGMA_REFLECTOR_COUNT];

    int plugboardSize = rand() % 11;
    for (int i = 0; i < plugboardSize * 2; i++) {
        unique = false;
        while (!unique) {
            unique = true;
            c      = 'A' + (rand() % ENIGMA_ALPHA_SIZE);
            for (int j = 0; j < i; j++) {
                if (enigma->plugboard[j] == c) {
                    unique = false;
                    break;
                }
            }
        }
        enigma->plugboard[i] = c;
    }

    enigma->plugboard[plugboardSize * 2] = '\0';
    return ENIGMA_SUCCESS;
}

/**
 * @brief Get the version of the Enigma library.
 *
 * This function returns the version of the Enigma library as a string.
 *
 * @return Pointer to the version string.
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_version(void) { return LIBENIGMA_VERSION; }

/**
 * @brief Get the plugboard configuration.
 *
 * This function returns the current plugboard configuration as a string.
 *
 * @param enigma Pointer to the Enigma machine.
 * @return Pointer to the plugboard configuration string.
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_get_plugboard(const enigma_t* enigma) {
    if (!enigma) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return enigma->plugboard;
}

/**
 * @brief Get the reflector configuration.
 *
 * This function returns the current reflector.
 *
 * @param enigma Pointer to the Enigma machine.
 * @return Pointer to the reflector configuration.
 */
EMSCRIPTEN_KEEPALIVE const enigma_reflector_t* enigma_get_reflector(const enigma_t* enigma) {
    if (!enigma) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    return enigma->reflector;
}

/**
 * @brief Get the rotor configuration.
 *
 * This function returns the rotor at the specified index.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param rotorIndex Index of the rotor to retrieve.
 * @return Pointer to the rotor configuration.
 */
EMSCRIPTEN_KEEPALIVE const enigma_rotor_t* enigma_get_rotor(const enigma_t* enigma,
                                                            int             rotorIndex) {
    if (!enigma) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return enigma->rotors[rotorIndex];
}

/**
 * @brief Get the number of rotors.
 *
 * This function returns the number of rotors in the Enigma machine.
 *
 * @param enigma Pointer to the Enigma machine.
 * @return Number of rotors, or ENIGMA_FAILURE if the Enigma machine is invalid.
 */
EMSCRIPTEN_KEEPALIVE int enigma_get_rotor_count(const enigma_t* enigma) {
    if (!enigma) {
        return ENIGMA_FAILURE;
    }
    return enigma->rotor_count;
}

/**
 * @brief Get the rotor flag.
 *
 * This function returns the rotor flag of the Enigma machine.
 *
 * @param enigma Pointer to the Enigma machine.
 * @return Rotor flag value, or ENIGMA_FAILURE if the Enigma machine is invalid.
 */
EMSCRIPTEN_KEEPALIVE int enigma_get_rotor_flag(const enigma_t* enigma) {
    if (!enigma) {
        return ENIGMA_FAILURE;
    }
    return enigma->rotor_flag;
}

/**
 * @brief Get the index of a rotor.
 *
 * This function returns the index of the specified rotor.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param rotor Pointer to the rotor configuration.
 * @return Index of the rotor, or ENIGMA_FAILURE if the Enigma machine is invalid.
 */
EMSCRIPTEN_KEEPALIVE int enigma_get_rotor_index(const enigma_t* enigma, int rotor) {
    if (!enigma) {
        return ENIGMA_FAILURE;
    }
    return enigma->rotor_indices[rotor];
}

/**
 * @brief Set the plugboard configuration.
 *
 * This function sets the plugboard configuration based on the provided string.
 * The string should contain pairs of uppercase letters representing the
 * connections between the letters. For example, "ABCDEF" would connect A to B,
 * C to D, and E to F.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param s Pointer to the string containing the plugboard configuration.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_set_plugboard(enigma_t* enigma, const char* s) {
    if (!enigma || !s || strlen(s) % 2 != 0 || strlen(s) > 26) {
        return ENIGMA_FAILURE;
    }
    int i = 0;
    while (s[i] != '\0') {
        char c = toupper(s[i]);
        if (c >= 'A' && c <= 'Z') {
            enigma->plugboard[i] = s[i];
            i++;
        } else {
            return ENIGMA_FAILURE;
        }
    }
    enigma->plugboard[i] = '\0';
    return ENIGMA_SUCCESS;
}

/**
 * Set the reflector configuration.
 *
 * This function sets the reflector configuration based on the provided enigma_reflectors index.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param reflector Index of the reflector to be set.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_set_reflector(enigma_t* enigma, int reflector) {
    if (!enigma || reflector < 0 || reflector >= ENIGMA_REFLECTOR_COUNT) {
        return ENIGMA_FAILURE;
    }
    enigma->reflector = enigma_reflectors[reflector];
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the rotor
 *
 * This function sets the rotor configuration based on the provided enigma_rotors index.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param rotor Index of the rotor in enigma_rotors.
 * @param index Index to place the rotor in enigma
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_set_rotor(enigma_t* enigma, int rotor, int index) {
    if (!enigma || rotor < 0 || rotor >= ENIGMA_ROTOR_COUNT || index < 0
        || index >= ENIGMA_MAX_ROTOR_COUNT) {
        return ENIGMA_FAILURE;
    }
    enigma->rotors[index] = enigma_rotors[rotor];
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the number of rotors.
 *
 * This function sets the number of rotors in the Enigma machine.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param count Number of rotors.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_set_rotor_count(enigma_t* enigma, int count) {
    if (!enigma || count < 0 || count > ENIGMA_MAX_ROTOR_COUNT) {
        return ENIGMA_FAILURE;
    }
    enigma->rotor_count = count;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the rotor flag status.
 *
 * This function sets the rotor flag status in the Enigma machine.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param flag Value to set the rotor flag to.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
int enigma_set_rotor_flag(enigma_t* enigma, int flag) {
    if (!enigma || flag < 0 || flag > 1) {
        return ENIGMA_FAILURE;
    }
    enigma->rotor_flag = flag;
    return ENIGMA_SUCCESS;
}
/**
 * @brief Set the index of a rotor.
 *
 * This function sets the index of a rotor in the Enigma machine.
 *
 * @param enigma Pointer to the Enigma machine.
 * @param rotor Index of the rotor in the Enigma machine.
 * @param index Index to set the rotor to in the Enigma machine.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_set_rotor_index(enigma_t* enigma, int rotor, int index) {
    if (!enigma || rotor < 0 || rotor >= enigma->rotor_count || index < 0
        || index >= ENIGMA_ALPHA_SIZE) {
        return ENIGMA_FAILURE;
    }
    enigma->rotor_indices[rotor] = index;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Rotate a single rotor by one position.
 *
 * This function increments the rotor's index, wrapping around if it exceeds the
 * alphabet size.
 *
 * @param rotor Pointer to the rotor to be rotated.
 */
static ENIGMA_ALWAYS_INLINE void rotate(int* idx) {
    (*idx)++;
    if (*idx == ENIGMA_ALPHA_SIZE) {
        *idx = 0;
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
static ENIGMA_ALWAYS_INLINE void rotate_rotors(enigma_t* enigma) {
    int turned = 0;
    for (int i = 0; i < enigma->rotors[1]->notches_count; i++) {
        if (enigma->rotors[1]->fwd_indices[enigma->rotor_indices[1]]
            == enigma->rotors[1]->notches[i]) {
            rotate(&enigma->rotor_indices[1]);
            rotate(&enigma->rotor_indices[2]);
            turned = 1;
            break;
        }
    }

    if (!turned) {
        for (int i = 0; i < enigma->rotors[0]->notches_count; i++) {
            if (enigma->rotors[0]->fwd_indices[enigma->rotor_indices[0]]
                == enigma->rotors[0]->notches[i]) {
                rotate(&enigma->rotor_indices[1]);
                break;
            }
        }
    }
    rotate(&enigma->rotor_indices[0]);
}

/**
 * @brief Pass through a rotor in the forward direction.
 *
 * @param rotor Pointer to the rotor struct.
 * @param rotIdx The rotor's current index.
 * @param idx The index of the character in the alphabet.
 *
 * @return The index of the character after passing through the rotor.
 */
static ENIGMA_ALWAYS_INLINE int
rotor_pass_forward(const enigma_rotor_t* rotor, int rotIdx, int idx) {
    idx = idx + rotIdx;
    if (idx >= ENIGMA_ALPHA_SIZE) {
        idx -= ENIGMA_ALPHA_SIZE;
    }

    idx = rotor->fwd_indices[idx];

    idx = (ENIGMA_ALPHA_SIZE + idx - rotIdx);
    if (idx >= ENIGMA_ALPHA_SIZE) {
        idx -= ENIGMA_ALPHA_SIZE;
    }
    return idx;
}

/**
 * @brief Pass through a rotor in the reverse direction.
 *
 * @param rotor Pointer to the rotor struct.
 * @param rotIdx The rotor's current index.
 * @param idx The index of the character in the alphabet.
 *
 * @return The index of the character after passing through the rotor.
 */
static ENIGMA_ALWAYS_INLINE int
rotor_pass_reverse(const enigma_rotor_t* rotor, int rotIdx, int idx) {
    idx += rotIdx;
    if (idx >= ENIGMA_ALPHA_SIZE) {
        idx -= ENIGMA_ALPHA_SIZE;
    }

    idx = rotor->rev_indices[idx];

    idx = (ENIGMA_ALPHA_SIZE + idx - rotIdx);
    if (idx >= ENIGMA_ALPHA_SIZE) {
        idx -= ENIGMA_ALPHA_SIZE;
    }
    return idx;
}

/**
 * @brief Substitute characters based on the plugboard configuration.
 *
 * This function takes in the plugboard configuration (an even-length string
 * representing pairs of characters to swap), and the character to be
 * substituted. The character must be uppercase.
 *
 * Assumes an even-length string.
 *
 * @param plugboard The plugboard configuration string.
 * @param c The character to be substituted.
 * @return The substituted character based on the plugboard configuration.
 */
static ENIGMA_ALWAYS_INLINE char substitute(const char* plugboard, char c) {
    if (!plugboard)
        return c;

    for (const char* p = plugboard; p[0]; p += 2) {
        if (p[0] == c)
            return p[1];
        if (p[1] == c)
            return p[0];
    }

    return c;
}
