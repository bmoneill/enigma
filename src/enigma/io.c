/**
 * @file enigma/io.c
 *
 * This file implements input/output functions.
 */
#include "io.h"

#include "crack.h"

#include <ctype.h>
#include <math.h>
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
 *
 * @return 0 on success, non-zero on failure.
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
                return 1;
            }
        }

        token = strtok(NULL, " ");
    }

    return 0;
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
        return 1;
    }

    for (int i = 0; i < enigma->rotor_count && s[i]; i++) {
        if (!isalpha(s[i])) {
            return 1;
        }
        enigma->rotors[i].idx = toupper(s[i]) - 'A';
    }

    return 0;
}

/**
 * @brief Load a custom rotor alphabet from the given alphabet, name, and notches.
 *
 * @param rotor Pointer to the rotor structure to be populated.
 * @param alphabet The custom rotor alphabet (must be 26 characters long).
 * @param name The name of the custom rotor.
 * @param notches Array of notch positions.
 * @param numNotches Number of notches.
 */
int enigma_load_custom_rotor(enigma_rotor_t* rotor, const char* alphabet, const char* name,
                             int* notches, int numNotches) {
    rotor->name = name;
    rotor->numNotches = numNotches;
    memcpy(rotor->notches, notches, numNotches * sizeof(int));
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        rotor->fwd_indices[i] = toupper(alphabet[i]) - 'A';
        rotor->rev_indices[rotor->fwd_indices[i]] = i;
    }
    return 1;
}

/**
 * @brief Load a custom reflector alphabet from the given alphabet and name.
 *
 * @param reflector Pointer to the reflector structure to be populated.
 * @param alphabet The custom reflector alphabet (must be 26 characters long).
 * @param name The name of the custom reflector.
 *
 * @return 0 on success, non-zero on failure.
 */
int enigma_load_custom_reflector(enigma_reflector_t* reflector, const char* alphabet, const char* name) {
    reflector->name = name;
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        reflector->indices[i] = toupper(alphabet[i]) - 'A';
    }
    return 0;
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
int enigma_load_ngrams(enigma_crack_config_t* cfg, const char* path) {
    char line[16];
    int n = 0;
    int lineCount = 0;
    int reallocCount = 0;
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open ngram file: %s\n", path);
    }


    if (fgets(line, sizeof(line), f)) {
        // First line should be n value and line count of original text
        if (sscanf(line, "%d %d", &n, &lineCount) != 2) {
            fprintf(stderr, "Invalid ngram file format: %s\n", path);
            fclose(f);
            return 0;
        }
    } else {
        fprintf(stderr, "Failed to read ngram file: %s\n", path);
        free(cfg->ngrams);
        fclose(f);
        return 0;
    }

    if (n > 4 || n < 1) {
        fprintf(stderr, "N-grams must be of size 2-4. Unsupported size: %d\n", n);
        free(cfg->ngrams);
        fclose(f);
        return 0;
    }
    cfg->ngrams = calloc(pow(26, n), sizeof(float));

    char s[5];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d %4s", &count, s) == 2) {
            switch (n) {
                case 2: cfg->ngrams[ENIGMA_BIIDX(s[0], s[1])] = (float)count / lineCount; break;
                case 3: cfg->ngrams[ENIGMA_TRIIDX(s[0], s[1], s[2])] = (float)count / lineCount; break;
                case 4: cfg->ngrams[ENIGMA_QUADIDX(s[0], s[1], s[2], s[3])] = (float)count / lineCount; break;
            }
        } else {
            break;
        }
    }

    fclose(f);
    return 1;
}

/**
 * @brief Load an Enigma machine configuration from a file.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param path Path to the configuration file.
 * @return 0 on success, non-zero on failure.
 */
int enigma_load_config(enigma_t* enigma, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open config file: %s\n", path);
        return 1;
    }

    char buf[256];
    if (!fgets(buf, sizeof(buf), f)) {
        fprintf(stderr, "Failed to read config file: %s\n", path);
        fclose(f);
        return 1;
    }

    char* rotors = strtok(buf, "|");
    char* positions = strtok(NULL, "|");
    char* reflector = strtok(NULL, "|");
    char* plugboard = strtok(NULL, "|");

    for (size_t i = 0; i < strlen(buf); i++) {
        if (buf[i] == '|') {
            buf[i] = '\0';
        }
    }

    if (enigma_load_rotor_config(enigma, rotors)) {
        fprintf(stderr, "Invalid rotor configuration: %s\n", rotors);
        fclose(f);
        return 1;
    }

    if (enigma_load_rotor_positions(enigma, positions)) {
        fprintf(stderr, "Invalid rotor positions: %s\n", positions);
        fclose(f);
        return 1;
    }

    if (enigma_load_reflector_config(enigma, reflector)) {
        fprintf(stderr, "Invalid reflector configuration: %s\n", reflector);
        fclose(f);
        return 1;
    }

    if (strcmp(enigma->plugboard, "None")) {
        enigma->plugboard = malloc(ENIGMA_ALPHA_SIZE + 1); // This won't be freed but it's okay
        strcpy(enigma->plugboard, plugboard);
    }


    fclose(f);
    return 0;
}

/**
 * @brief Save the current Enigma machine configuration to a file.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param path Path to the configuration file.
 * @return 0 on success, non-zero on failure.
 */
int enigma_save_config(const enigma_t* enigma, const char* path) {
    char buf[80];
    FILE* f = fopen(path, "w");

    if (!f) {
        fprintf(stderr, "Failed to open config file for writing: %s\n", path);
        return 1;
    }

    enigma_print_config(enigma, buf);
    fprintf(f, "%s\n", buf);
    fclose(f);
    return 0;
}

/**
 * @brief Print the current Enigma machine configuration to out.
 *
 * @param enigma Pointer to the Enigma machine instance.
 * @param out    Buffer to store the configuration string.
 */
void enigma_print_config(const enigma_t* enigma, char* out) {
    sprintf(out, "%s %s %s|%c%c%c|%s|%s",
            enigma->rotors[0].name, enigma->rotors[1].name, enigma->rotors[2].name,
            enigma->rotors[0].idx + 'A', enigma->rotors[1].idx + 'A', enigma->rotors[2].idx + 'A',
            enigma->reflector.name, enigma->plugboard ? enigma->plugboard : "None");
}
