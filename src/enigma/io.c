/**
 * @file enigma/io.c
 *
 * This file implements input/output functions.
 */
#include "io.h"

#include "common.h"
#include "crack.h"
#include "enigma.h"
#include "ngram.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ipow(int, int);

/**
 * @brief Print an error message to stderr.
 *
 * @param format Format string.
 * @param ...    Arguments for the format string.
 * @return       ENIGMA_FAILURE.
 */
int enigma_error_message(const char* func, const char* format, ...) {
    fprintf(stderr, "libenigma (%s): ", func);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    return ENIGMA_FAILURE;
}

/**
 * @brief Load an Enigma machine configuration from a string.
 *
 * The configuration string should be in the format:
 * "ROTORS|POSITIONS|REFLECTOR|PLUGBOARD",
 * where ROTORS is a space-separated list of rotor names,
 * POSITIONS is a string of starting positions for each rotor,
 * REFLECTOR is the reflector name, and PLUGBOARD is a string
 * representing plugboard connections. If there is no plugboard,
 * use "None".
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing the Enigma configuration.
 * @return       ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_config(enigma_t* enigma, const char* s) {
    char buf[64];
    if (strlen(s) >= sizeof(buf)) {
        return ENIGMA_ERROR("Configuration string too long: %s", s);
    }
    strcpy(buf, s);

    char* rotors    = strtok(buf, "|");
    char* positions = strtok(NULL, "|");
    char* reflector = strtok(NULL, "|");
    char* plugboard = strtok(NULL, "|");

    for (size_t i = 0; i < strlen(buf); i++) {
        if (buf[i] == '|') {
            buf[i] = '\0';
        }
    }

    if (enigma_load_rotor_config(enigma, rotors)) {
        return ENIGMA_ERROR("Invalid rotor configuration: %s", rotors);
    }

    if (enigma_load_rotor_positions(enigma, positions)) {
        return ENIGMA_ERROR("Invalid rotor positions: %s", positions);
    }

    if (enigma_load_reflector_config(enigma, reflector)) {
        return ENIGMA_ERROR("Invalid reflector configuration: %s", reflector);
    }

    if (strcmp(enigma->plugboard, "None")) {
        strcpy(enigma->plugboard, plugboard);
    }

    return ENIGMA_SUCCESS;
}

/**
 * @brief Load a custom reflector alphabet from the given alphabet and name.
 *
 * @param reflector Pointer to the reflector structure to be populated.
 * @param alphabet The custom reflector alphabet (must be 26 characters long).
 * @param name The name of the custom reflector.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_custom_reflector(enigma_reflector_t* reflector,
                                                      const char*         alphabet,
                                                      const char*         name) {
    if (strlen(alphabet) != ENIGMA_ALPHA_SIZE) {
        return ENIGMA_FAILURE;
    }

    reflector->name = name;
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        reflector->indices[i] = toupper(alphabet[i]) - 'A';
    }
    return ENIGMA_SUCCESS;
}

/**
 * @brief Load a custom rotor alphabet from the given alphabet, name, and notches.
 *
 * @param rotor Pointer to the rotor structure to be populated.
 * @param alphabet The custom rotor alphabet (must be 26 characters long).
 * @param name The name of the custom rotor.
 * @param notches Array of notch positions (length must match numNotches).
 * @param numNotches Number of notches.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_custom_rotor(
    enigma_rotor_t* rotor, const char* alphabet, const char* name, int* notches, int numNotches) {
    if (strlen(alphabet) != ENIGMA_ALPHA_SIZE) {
        return ENIGMA_FAILURE;
    }
    rotor->name          = name;
    rotor->notches_count = numNotches;
    memcpy(rotor->notches, notches, numNotches * sizeof(int));
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        rotor->fwd_indices[i]                     = toupper(alphabet[i]) - 'A';
        rotor->rev_indices[rotor->fwd_indices[i]] = i;
    }

    return ENIGMA_SUCCESS;
}

/**
 * @brief Load ngrams from a file.
 *
 * The file should have the following format:
 * First line: <n> <charCount>
 * Subsequent lines: <count> <ngram>
 *
 * @param cfg Pointer to the cracking configuration structure.
 * @param path Path to the ngram file.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_ngrams(enigma_crack_t* cfg, const char* path) {
    char  line[16];
    int   n            = 0;
    int   charCount    = 0;
    int   reallocCount = 0;
    FILE* f            = fopen(path, "r");
    if (!f) {
        return ENIGMA_ERROR("Failed to open ngram file: %s", path);
    }

    if (fgets(line, sizeof(line), f)) {
        // First line should be n value and character count of original text
        if (sscanf(line, "%d %d", &n, &charCount) != 2) {
            ENIGMA_ERROR("Invalid ngram file format: %s", path);
            fclose(f);
            return ENIGMA_FAILURE;
        }
    } else {
        ENIGMA_ERROR("Failed to read ngram file: %s", path);
        free(cfg->ngrams);
        fclose(f);
        return ENIGMA_FAILURE;
    }

    if (n > 4 || n < 1) {
        ENIGMA_ERROR("N-grams must be of size 2-4. Unsupported size: %d", n);
        free(cfg->ngrams);
        fclose(f);
        return ENIGMA_FAILURE;
    }
    cfg->ngrams = calloc(ipow(26, n), sizeof(float));
    cfg->n      = n;

    char s[5];
    int  count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d %4s", &count, s) == 2) {
            switch (n) {
            case 2:
                cfg->ngrams[ENIGMA_BIIDX(s[0], s[1])] = (float) count / charCount;
                break;
            case 3:
                cfg->ngrams[ENIGMA_TRIIDX(s[0], s[1], s[2])] = (float) count / charCount;
                break;
            case 4:
                cfg->ngrams[ENIGMA_QUADIDX(s[0], s[1], s[2], s[3])] = (float) count / charCount;
                break;
            }
        } else {
            break;
        }
    }

    fclose(f);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Load plugboard configuration from a string.
 *
 * The string should contain non-separated pairs of characters representing the plugboard
 * connections, e.g., "ABCDEF" means A<->B, C<->D, E<->F.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing plugboard configuration.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_plugboard_config(enigma_t* enigma, const char* s) {
    int len = strlen(s);
    if (len % 2 != 0 || len > ENIGMA_ALPHA_SIZE) {
        return ENIGMA_ERROR("Invalid plugboard configuration: %s", s);
    }
    memcpy(enigma->plugboard, s, len);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Load reflector configuration from a string.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing plugboard configuration.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_reflector_config(enigma_t* enigma, const char* s) {
    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        if (!strcmp(enigma_reflectors[i]->name, s)) {
            enigma->reflector = enigma_reflectors[i];
            return ENIGMA_SUCCESS;
        }
    }
    return ENIGMA_FAILURE;
}

/**
 * @brief Load rotor configuration from a string.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing rotor configuration.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_rotor_config(enigma_t* enigma, char* s) {
    enigma->rotor_count = 0;

    char* token         = strtok(s, " ");
    while (token != NULL) {
        for (int i = 0; i < ENIGMA_ROTOR_COUNT; i++) {
            if (!strcmp(enigma_rotors[i]->name, token)) {
                enigma->rotors[enigma->rotor_count++] = enigma_rotors[i];
                break;
            }

            if (i == ENIGMA_ROTOR_COUNT - 1) {
                return ENIGMA_ERROR("Invalid rotor configuration: %s", s);
            }
        }

        token = strtok(NULL, " ");
    }

    return ENIGMA_SUCCESS;
}

/**
 * @brief Load rotor starting positions from a string.
 *
 * This function expects a string of characters representing the initial positions
 * of the rotors, e.g., "ABC" for three rotors. It sets the `idx` field of each rotor
 * in the `enigma_t` structure.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing rotor starting positions.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_load_rotor_positions(enigma_t* enigma, const char* s) {
    if (enigma->rotor_count == 0) {
        return ENIGMA_ERROR("%s", "Cannot load rotor positions without rotors");
    }

    for (int i = 0; i < enigma->rotor_count && s[i]; i++) {
        if (!isalpha(s[i])) {
            return ENIGMA_FAILURE;
        }
        enigma->rotor_indices[i] = toupper(s[i]) - 'A';
    }

    return ENIGMA_SUCCESS;
}

/**
 * @brief Print the current Enigma machine configuration to out.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param out    Buffer to store the configuration string.
 */
EMSCRIPTEN_KEEPALIVE void enigma_print_config(const enigma_t* enigma, char* out) {
    sprintf(out,
            "%s %s %s|%c%c%c|%s|%s",
            enigma->rotors[0]->name,
            enigma->rotors[1]->name,
            enigma->rotors[2]->name,
            enigma->rotor_indices[0] + 'A',
            enigma->rotor_indices[1] + 'A',
            enigma->rotor_indices[2] + 'A',
            enigma->reflector->name,
            enigma->plugboard[0] == '\0' ? "None" : enigma->plugboard);
}

/**
 * @brief Calculate base raised to the power of exp.
 *
 * @param base The base integer.
 * @param exp The exponent integer.
 * @return The result of base raised to the power of exp.
 */
static int ipow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}
