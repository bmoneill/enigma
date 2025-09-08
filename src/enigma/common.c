#include "common.h"

#include <ctype.h>
#include <string.h>

static bool enigma_verbose = false;

/**
 * @brief Load the reflector configuration from its string identifier.
 *
 * This function searches through the available reflectors and sets the
 * `reflector` field of the `enigma_t` to the one matching the provided identifier.
 * It assumes the memory for the reflector has already been allocated.
 *
 * @param enigma Pointer to the `enigma_t`.
 * @param s The string identifier for the reflector.
 * @return 1 if the reflector was found and loaded, 0 otherwise.
 */
int enigma_load_reflector_config(enigma_t* enigma, const char* s) {
    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        if (!strcmp(enigma_reflectors[i]->name, s)) {
            memcpy(&enigma->reflector, enigma_reflectors[i], sizeof(enigma_reflector_t));
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Load the rotor configuration from string identifiers.
 *
 * This function parses a space-separated string of rotor names and populates
 * the `rotors` array in the `enigma_t` (for CLI argument parsing).
 *
 * @param enigma Pointer to the `enigma_t`.
 * @param s The string containing rotor names.
 *
 * @return 1 if the rotors were successfully loaded, 0 otherwise.
 */
int enigma_load_rotor_config(enigma_t* enigma, char* s) {
    enigma->rotor_count = 0;

    char* token = strtok(s, " ");
    while (token != NULL) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            if (!strcmp(enigma_rotors[i]->name, token)) {
                enigma->rotors[enigma->rotor_count++] = *enigma_rotors[i];
                break;
            }

            if (i == ENIGMA_ROTOR_COUNT - 1) {
                return 0;
            }
        }

        token = strtok(NULL, " ");
    }

    return 1;
}

/**
 * @brief Load the initial positions of the rotors from a string.
 *
 * This function expects a string of characters representing the initial positions
 * of the rotors, e.g., "ABC" for three rotors. It sets the `idx` field of each rotor
 * in the `enigma_t` structure.
 *
 * @param enigma Pointer to the `enigma_t`.
 * @param s The string containing rotor positions.
 *
 * @return 1 if the positions were successfully loaded, 0 otherwise.
 */
int enigma_load_rotor_positions(enigma_t* enigma, char* s) {
    if (enigma->rotor_count == 0) {
        return 0;
    }

    for (int i = 0; i < enigma->rotor_count && s[i]; i++) {
        enigma->rotors[i].idx = toupper(s[i]) - 'A';
    }

    return 1;
}
