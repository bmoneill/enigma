#include "rotor.h"

#include "common.h"
#include "io.h"

#include <ctype.h>
#include <string.h>

/**
 * @brief Generate the indices for the rotor.
 *
 * This function generates the indices for the rotor.
 *
 * @param rotor The rotor to generate the indices for.
 * @param alphabet The alphabet to generate the indices from.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_rotor_generate_indices(enigma_rotor_t* rotor,
                                                       const char*     alphabet) {
    if (!rotor || !alphabet || strlen(alphabet) != 26) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    for (int i = 0; i < 26; i++) {
        rotor->fwd_indices[i] = toupper(alphabet[i]) - 'A';
    }

    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            if (toupper(alphabet[i]) - 'A' == j) {
                rotor->rev_indices[i] = j;
            }
        }
    }

    return ENIGMA_SUCCESS;
}

/**
 * @brief Get the name of the rotor.
 *
 * This function gets the name of the rotor.
 *
 * @param rotor The rotor to set the name of.
 * @param name The name to set.
 * @return the name of the rotor on success, or NULL on failure.
 */
EMSCRIPTEN_KEEPALIVE const char* enigma_rotor_get_name(const enigma_rotor_t* rotor) {
    if (!rotor) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return rotor->name;
}

/**
 * @brief Get the forward indices of the rotor.
 *
 * This function gets the forward indices of the rotor.
 *
 * @param rotor The rotor to get the forward indices of.
 * @return the forward indices of the rotor on success, or NULL on failure.
 */
EMSCRIPTEN_KEEPALIVE const int* enigma_rotor_get_fwd_indices(const enigma_rotor_t* rotor) {
    if (!rotor) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return rotor->fwd_indices;
}

/**
 * @brief Get the reverse indices of the rotor.
 *
 * This function gets the reverse indices of the rotor.
 *
 * @param rotor The rotor to get the reverse indices of.
 * @return the reverse indices of the rotor on success, or NULL on failure.
 */
EMSCRIPTEN_KEEPALIVE const int* enigma_rotor_get_rev_indices(const enigma_rotor_t* rotor) {
    if (!rotor) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return rotor->rev_indices;
}

/**
 * @brief Get the notches of the rotor.
 *
 * This function gets the notches of the rotor.
 *
 * @param rotor The rotor to get the notches of.
 * @return the notches of the rotor on success, or NULL on failure.
 */
EMSCRIPTEN_KEEPALIVE const int* enigma_rotor_get_notches(const enigma_rotor_t* rotor) {
    if (!rotor) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }
    return rotor->notches;
}

/**
 * @brief Get the number of notches of the rotor.
 *
 * This function gets the number of notches of the rotor.
 *
 * @param rotor The rotor to get the number of notches of.
 * @return the number of notches of the rotor on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_rotor_get_notches_count(const enigma_rotor_t* rotor) {
    if (!rotor) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    return rotor->notches_count;
}

/**
 * @brief Set the name of the rotor.
 *
 * This function sets the name of the rotor (uses strdup, make sure to free this memory when done).
 *
 * @param rotor The rotor to set the name of.
 * @param name The name to set.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_rotor_set_name(enigma_rotor_t* rotor, const char* name) {
    if (!rotor || !name) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    rotor->name = strdup(name);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the forward indices of the rotor.
 *
 * This function sets the forward indices of the rotor.
 *
 * @param rotor The rotor to set the forward indices of.
 * @param indices The forward indices to set.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_rotor_set_fwd_indices(enigma_rotor_t* rotor, const int* indices) {
    if (!rotor || !indices) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    memcpy(rotor->fwd_indices, indices, sizeof(int) * 26);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the reverse indices of the rotor.
 *
 * This function sets the reverse indices of the rotor.
 *
 * @param rotor The rotor to set the reverse indices of.
 * @param indices The reverse indices to set.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_rotor_set_rev_indices(enigma_rotor_t* rotor, const int* indices) {
    if (!rotor || !indices) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    memcpy(rotor->rev_indices, indices, sizeof(int) * 26);
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the notches of the rotor.
 *
 * This function sets the notches of the rotor.
 *
 * @param rotor The rotor to set the notches of.
 * @param notches The notches to set.
 * @param count The number of notches.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int
enigma_rotor_set_notches(enigma_rotor_t* rotor, const int* notches, int count) {
    if (!rotor || !notches || count < 0 || count > 2) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    memcpy(rotor->notches, notches, sizeof(int) * count);
    rotor->notches_count = count;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the notches count of the rotor.
 *
 * This function sets the notches count of the rotor.
 *
 * @param rotor The rotor to set the notches count of.
 * @param count The notches count to set.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_rotor_set_notches_count(enigma_rotor_t* rotor, int count) {
    if (!rotor || count < 0 || count > 2) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }
    rotor->notches_count = count;
    return ENIGMA_SUCCESS;
}
