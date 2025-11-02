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
 * @return 0 on success, -1 on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_reflector_generate_indices(enigma_reflector_t* reflector,
                                                           const char*         alphabet) {
    if (!reflector || !alphabet || strlen(alphabet) != 26) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    for (int i = 0; i < 26; i++) {
        reflector->indices[i] = toupper(alphabet[i]) - 'A';
    }
    return 0;
}
/**
 * @brief Get the name of the reflector.
 *
 * @param reflector The reflector to get the name of.
 * @return The name of the reflector.
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_reflector_get_name(const enigma_reflector_t* reflector) {
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
EMSCRIPTEN_KEEPALIVE const int* enigma_reflector_get_indices(const enigma_reflector_t* reflector) {
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
 * @return 0 on success, -1 on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_reflector_set_name(enigma_reflector_t* reflector,
                                                   const char*         name) {
    if (!reflector || !name) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    reflector->name = strdup(name);
    return 0;
}

/**
 * @brief Set the indices of the reflector.
 *
 * @param reflector The reflector to set the indices of.
 * @param indices The indices to set.
 * @return 0 on success, -1 on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_reflector_set_indices(enigma_reflector_t* reflector,
                                                      const int*          indices) {
    if (!reflector || !indices) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    memcpy(reflector->indices, indices, sizeof(int) * 26);
    return 0;
}
