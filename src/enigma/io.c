#include "io.h"

#include "crack.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Load plugboard configuration from a string.
 *
 * The string should contain non-separated pairs of characters representing the plugboard
 * connections, e.g., "ABCDEF" means A<->B, C<->D, E<->F.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing plugboard configuration.
 *
 * @return 0 on success, non-zero on failure.
 */
int enigma_load_plugboard_config(enigma_t* enigma, const char* s) {
    int len = strlen(s);
    if (len % 2 != 0 || len > ENIGMA_ALPHA_SIZE) {
        return 1;
    }
    memcpy(enigma->plugboard, s, len);
    return 0;
}

/**
 * @brief Load reflector configuration from a string.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing plugboard configuration.
 *
 * @return 0 on success, non-zero on failure.
 */
int enigma_load_reflector_config(enigma_t* enigma, const char* s) {
    for (int i = 0; i < ENIGMA_REFLECTOR_COUNT; i++) {
        if (!strcmp(enigma_reflectors[i]->name, s)) {
            memcpy(&enigma->reflector, enigma_reflectors[i], sizeof(enigma_reflector_t));
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Load rotor configuration from a string.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing rotor configuration.
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
 * @brief Load rotor starting positions from a string.
 *
 * This function expects a string of characters representing the initial positions
 * of the rotors, e.g., "ABC" for three rotors. It sets the `idx` field of each rotor
 * in the `enigma_t` structure.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param s      String representing rotor starting positions.
 *
 * @return 0 on success, non-zero on failure.
 */
int enigma_load_rotor_positions(enigma_t* enigma, const char* s) {
    if (enigma->rotor_count == 0) {
        return 0;
    }

    for (int i = 0; i < enigma->rotor_count && s[i]; i++) {
        enigma->rotors[i].idx = toupper(s[i]) - 'A';
    }

    return 1;
}

/**
 * @brief Load a custom rotor alphabet from the given alphabet, name, and notches.
 *
 * Currently unimplemented.
 *
 * @param rotor Pointer to the rotor structure to be populated.
 * @param alphabet The custom rotor alphabet.
 * @param name The name of the custom rotor.
 * @param notches Array of notch positions.
 * @param numNotches Number of notches.
 * @todo Implement this function.
 */
int enigma_load_custom_rotor(enigma_rotor_t* rotor, const char* alphabet, const char* name,
                             int* notches, int numNotches) {
    // TODO Implement
    return 1;
}

/**
 * @brief Load a custom reflector alphabet from the given alphabet and name.
 *
 * Currently unimplemented.
 *
 * @param reflector Pointer to the reflector structure to be populated.
 * @param alphabet The custom reflector alphabet.
 * @param name The name of the custom reflector.
 *
 * @return 0 on success, non-zero on failure.
 * @todo Implement this function.
 */
int enigma_load_custom_reflector(enigma_reflector_t* reflector, const char* alphabet, const char* name) {
    // TODO implement
    return 1;
}

/**
 * @brief Load ngrams from a file.
 *
 * The file should have the following format:
 * First line: <n> <lineCount>
 * Subsequent lines: <count> <ngram>
 *
 * @param path Path to the ngram file.
 * @return enigma_ngram_list_t* Pointer to the loaded ngram list, or NULL on failure.
 */
enigma_ngram_list_t* enigma_load_ngrams(const char* path) {
    char line[16];
    int lineCount = 0;
    int reallocCount = 0;
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open ngram file: %s\n", path);
    }

    enigma_ngram_list_t* ngram_list = malloc(sizeof(enigma_ngram_list_t));
    ngram_list->ngrams = malloc(ENIGMA_DEFAULT_NGRAM_COUNT * sizeof(enigma_ngram_t));
    ngram_list->count = 0;
    ngram_list->n = -1;

    if (fgets(line, sizeof(line), f)) {
        // First line should be n value and line count of original text
        if (sscanf(line, "%d %d", &ngram_list->n, &lineCount) != 2) {
            fprintf(stderr, "Invalid ngram file format: %s\n", path);
            free(ngram_list->ngrams);
            free(ngram_list);
            fclose(f);
            return NULL;
        }
    }
    else {
        fprintf(stderr, "Failed to read ngram file: %s\n", path);
        free(ngram_list->ngrams);
        free(ngram_list);
        fclose(f);
        return NULL;
    }

    while (fgets(line, sizeof(line), f)) {
        if (ngram_list->count >= ENIGMA_DEFAULT_NGRAM_COUNT * (reallocCount + 1)) {
            ngram_list->ngrams = realloc(ngram_list->ngrams,
                                         (ngram_list->count + ENIGMA_DEFAULT_NGRAM_COUNT
                                          * ++reallocCount) * sizeof(enigma_ngram_t));
        }
        if (sscanf(line, "%d %4s", &ngram_list->ngrams[ngram_list->count].count, ngram_list->ngrams[ngram_list->count].ngram) == 2) {
            ngram_list->count++;
        }
    }

    fclose(f);
    return ngram_list;
}

/**
 * @brief Load an Enigma machine configuration from a file.
 *
 * Currently unimplemented.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param path Path to the configuration file.
 * @return 0 on success, non-zero on failure.
 * @todo Implement this function.
 */
int enigma_load_config(enigma_t* enigma, const char* path) {
    // TODO implement
    return 1;
}

/**
 * @brief Save the current Enigma machine configuration to a file.
 *
 * Currently unimplemented.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param path Path to the configuration file.
 * @return 0 on success, non-zero on failure.
 * @todo Implement this function.
 */
int enigma_save_config(const enigma_t* enigma, const char* path) {
    // TODO implement
    return 1;
}

/**
 * @brief Print the current Enigma machine configuration to out.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param out    Buffer to store the configuration string.
 */
void enigma_print_config(const enigma_t* enigma, char* out) {
    sprintf(out, "Rotors: %s (%c)  %s (%c), %s (%c) | Reflector: %s | Plugboard: %s\n",
            enigma->rotors[0].name, enigma->rotors[0].idx + 'A',
            enigma->rotors[1].name, enigma->rotors[1].idx + 'A',
            enigma->rotors[2].name, enigma->rotors[2].idx + 'A',
            enigma->reflector.name, enigma->plugboard ? enigma->plugboard : "None");
}
