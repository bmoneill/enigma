#include "score.h"

#include "enigma/common.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

static int score_compare(const void* a, const void* b);

/**
 * @brief Print the scores in an enigma_score_list_t.
 *
 * @param list Pointer to the enigma_score_list_t structure.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_print(const enigma_score_list_t* list) {
    if (!list) {
        return ENIGMA_FAILURE;
    }
    char buf[64];
    for (int i = 0; i < list->score_count; i++) {
        printf("%.6f ", list->scores[i].score);
        if (list->scores[i].flags & ENIGMA_FLAG_DICTIONARY_MATCH) {
            printf("D");
        } else {
            printf("-");
        }
        if (list->scores[i].flags & ENIGMA_FLAG_FREQUENCY) {
            printf("F ");
        } else {
            printf("- ");
        }
        enigma_print_config(&list->scores[i].enigma, buf);
        printf("%s\n", buf);
    }
    return ENIGMA_SUCCESS;
}

/**
 * @brief Sort an array of enigma_score_t by score in descending order.
 *
 * @param list The array of enigma_score_t to sort.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_sort(enigma_score_list_t* list) {
    if (!list) {
        return ENIGMA_FAILURE;
    }

    qsort(list->scores, list->score_count, sizeof(enigma_score_t), score_compare);
    return ENIGMA_SUCCESS;
}

/**
  * @brief Get the enigma field in the given enigma_score_t
  *
  * @param score The enigma_score_t instance
  * @return The enigma field, or NULL if score is NULL
  */
EMSCRIPTEN_KEEPALIVE enigma_t* enigma_score_get_enigma(enigma_score_t* score) {
    if (!score) {
        return NULL;
    }

    return &score->enigma;
}

/**
 * @brief Get the score field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @return The score field, or ENIGMA_FAILURE if score is NULL
 */
EMSCRIPTEN_KEEPALIVE float enigma_score_get_score(enigma_score_t* score) {
    if (!score) {
        return (float) ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return score->score;
}

/**
 * @brief Get the flags field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @return The flags field, or ENIGMA_FAILURE if score is NULL
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_get_flags(enigma_score_t* score) {
    if (!score) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return score->flags;
}

/**
 * @brief Set the enigma field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @param enigma The enigma_t instance to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_enigma(enigma_score_t* score, enigma_t* enigma) {
    if (!score || !enigma) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    score->enigma = *enigma;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the score field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @param value The score value to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_score(enigma_score_t* score, float value) {
    if (!score) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    score->score = value;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the flags field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @param flags The flags value to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_flags(enigma_score_t* score, int flags) {
    if (!score) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    score->flags = flags;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Get the scores array from the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @return enigma_score_t* The scores array
 */
EMSCRIPTEN_KEEPALIVE enigma_score_t* enigma_score_list_get_scores(enigma_score_list_t* list) {
    if (!list) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return list->scores;
}

/**
 * @brief Get the score at the given index from the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @param index The index of the score to get
 * @return enigma_score_t* The score at the given index
 */
EMSCRIPTEN_KEEPALIVE enigma_score_t* enigma_score_list_get_score(enigma_score_list_t* list,
                                                                 int                  index) {
    if (!list || index < 0 || index >= list->score_count) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return &list->scores[index];
}

/**
 * @brief Get the score count from the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @return int The score count
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_get_score_count(enigma_score_list_t* list, int index) {
    if (!list) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return list->score_count;
}

/**
 * @brief Get the maximum number of scores from the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @return int The maximum number of scores
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_get_max_scores(enigma_score_list_t* list) {
    if (!list) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return list->max_scores;
}

/**
 * @brief Set the score at the given index in the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @param index The index of the score to set
 * @param score The enigma_score_t instance to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int
enigma_score_list_set_score(enigma_score_list_t* list, int index, enigma_score_t* score) {
    if (!list || !score || index < 0 || index >= list->score_count) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    list->scores[index] = *score;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the score count in the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @param count The new score count
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_set_score_count(enigma_score_list_t* list, int count) {
    if (!list || count < 0) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    list->score_count = count;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the maximum number of scores in the given enigma_score_list_t
 *
 * @param list The enigma_score_list_t instance
 * @param max The new maximum number of scores
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_set_max_scores(enigma_score_list_t* list, int max) {
    if (!list || max < 0) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    list->max_scores = max;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Compare function for sorting enigma_score_t by score.
 *
 * This function is used by qsort to sort an array of enigma_score_t structures
 * in descending order based on the score field.
 */
static int score_compare(const void* a, const void* b) {
    const enigma_score_t* scoreA = (const enigma_score_t*) a;
    const enigma_score_t* scoreB = (const enigma_score_t*) b;

    if (scoreA->score < scoreB->score) {
        return 1;
    } else if (scoreA->score > scoreB->score) {
        return -1;
    } else {
        return 0;
    }
}
