/**
 * @file enigma/reflector.c
 *
 * This file contains functions for reflector manipulation and getters/setters for WASM.
 */
#include "reflector.h"

#include "common.h"
#include "io.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>

/**
 * @brief Generate the indices for the reflector.
 *
 * @param reflector The reflector to generate the indices for.
 * @param alphabet The alphabet to generate the indices from.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_reflector_generate_indices(EnigmaReflector* reflector,
                                                           const char*      alphabet) {
    if (!reflector || !alphabet || strlen(alphabet) != 26) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    for (int i = 0; i < 26; i++) {
        reflector->indices[i] = toupper(alphabet[i]) - 'A';
    }
    return ENIGMA_SUCCESS;
}
/**
 * @brief Get the name of the reflector.
 *
 * @param reflector The reflector to get the name of.
 * @return The name of the reflector.
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_reflector_get_name(const EnigmaReflector* reflector) {
    if (!reflector) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return reflector->name;
}

/**
 * @brief Get the indices of the reflector.
 *
 * @param reflector The reflector to get the indices of.
 * @return The indices of the reflector.
 */
EMSCRIPTEN_KEEPALIVE const int* enigma_reflector_get_indices(const EnigmaReflector* reflector) {
    if (!reflector) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return reflector->indices;
}

/**
 * @brief Set the name of the reflector.
 *
 * This function sets the name of the reflector (uses strdup, make sure to free this memory when
 * done).
 *
 * @param reflector The reflector to set the name of.
 * @param name The name to set.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_reflector_set_name(EnigmaReflector* reflector, const char* name) {
    if (!reflector || !name) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    reflector->name = strdup(name);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the indices of the reflector.
 *
 * @param reflector The reflector to set the indices of.
 * @param indices The indices to set.
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_reflector_set_indices(EnigmaReflector* reflector,
                                                      const int*       indices) {
    if (!reflector || !indices) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    memcpy(reflector->indices, indices, sizeof(int) * 26);
    return ENIGMA_SUCCESS;
}
