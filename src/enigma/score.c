#include "score.h"

#include "common.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

static int score_compare(const void* a, const void* b);

/**
 * @brief Print the scores in an EnigmaScoreList.
 *
 * @param list Pointer to the EnigmaScoreList structure.
 * @return ENIGMA_SUCCESS on success, or ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_print(const EnigmaScoreList* list) {
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
 * @brief Sort an array of EnigmaScore by score in descending order.
 *
 * @param list The array of EnigmaScore to sort.
 *
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_sort(EnigmaScoreList* list) {
    if (!list) {
        return ENIGMA_FAILURE;
    }

    qsort(list->scores, list->score_count, sizeof(EnigmaScore), score_compare);
    return ENIGMA_SUCCESS;
}

/**
  * @brief Get the enigma field in the given EnigmaScore
  *
  * @param score The EnigmaScore instance
  * @return The enigma field, or NULL if score is NULL
  */
EMSCRIPTEN_KEEPALIVE Enigma* enigma_score_get_enigma(EnigmaScore* score) {
    if (!score) {
        return NULL;
    }

    return &score->enigma;
}

/**
 * @brief Get the score field in the given EnigmaScore
 *
 * @param score The EnigmaScore instance
 * @return The score field, or ENIGMA_FAILURE if score is NULL
 */
EMSCRIPTEN_KEEPALIVE float enigma_score_get_score(EnigmaScore* score) {
    if (!score) {
        return (float) ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return score->score;
}

/**
 * @brief Get the flags field in the given EnigmaScore
 *
 * @param score The EnigmaScore instance
 * @return The flags field, or ENIGMA_FAILURE if score is NULL
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_get_flags(EnigmaScore* score) {
    if (!score) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return score->flags;
}

/**
 * @brief Set the enigma field in the given EnigmaScore
 *
 * @param score The EnigmaScore instance
 * @param enigma The Enigma instance to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_enigma(EnigmaScore* score, Enigma* enigma) {
    if (!score || !enigma) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    score->enigma = *enigma;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the score field in the given EnigmaScore
 *
 * @param score The EnigmaScore instance
 * @param value The score value to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_score(EnigmaScore* score, float value) {
    if (!score) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    score->score = value;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the flags field in the given EnigmaScore
 *
 * @param score The EnigmaScore instance
 * @param flags The flags value to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_flags(EnigmaScore* score, int flags) {
    if (!score) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    score->flags = flags;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Get the scores array from the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @return EnigmaScore* The scores array
 */
EMSCRIPTEN_KEEPALIVE EnigmaScore* enigma_score_list_get_scores(EnigmaScoreList* list) {
    if (!list) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return list->scores;
}

/**
 * @brief Get the score at the given index from the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @param index The index of the score to get
 * @return EnigmaScore* The score at the given index
 */
EMSCRIPTEN_KEEPALIVE EnigmaScore* enigma_score_list_get_score(EnigmaScoreList* list, int index) {
    if (!list || index < 0 || index >= list->score_count) {
        ENIGMA_ERROR("%s", enigma_invalid_argument_message);
        return NULL;
    }

    return &list->scores[index];
}

/**
 * @brief Get the score count from the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @return int The score count
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_get_score_count(EnigmaScoreList* list, int index) {
    if (!list) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return list->score_count;
}

/**
 * @brief Get the maximum number of scores from the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @return int The maximum number of scores
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_get_max_scores(EnigmaScoreList* list) {
    if (!list) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    return list->max_scores;
}

/**
 * @brief Set the score at the given index in the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @param index The index of the score to set
 * @param score The EnigmaScore instance to set
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int
enigma_score_list_set_score(EnigmaScoreList* list, int index, EnigmaScore* score) {
    if (!list || !score || index < 0 || index >= list->score_count) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    list->scores[index] = *score;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the score count in the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @param count The new score count
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_set_score_count(EnigmaScoreList* list, int count) {
    if (!list || count < 0) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    list->score_count = count;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Set the maximum number of scores in the given EnigmaScoreList
 *
 * @param list The EnigmaScoreList instance
 * @param max The new maximum number of scores
 * @return ENIGMA_SUCCESS on success, ENIGMA_FAILURE on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_list_set_max_scores(EnigmaScoreList* list, int max) {
    if (!list || max < 0) {
        return ENIGMA_ERROR("%s", enigma_invalid_argument_message);
    }

    list->max_scores = max;
    return ENIGMA_SUCCESS;
}

/**
 * @brief Compare function for sorting EnigmaScore by score.
 *
 * This function is used by qsort to sort an array of EnigmaScore structures
 * in descending order based on the score field.
 */
static int score_compare(const void* a, const void* b) {
    const EnigmaScore* scoreA = (const EnigmaScore*) a;
    const EnigmaScore* scoreB = (const EnigmaScore*) b;

    if (scoreA->score < scoreB->score) {
        return 1;
    } else if (scoreA->score > scoreB->score) {
        return -1;
    } else {
        return 0;
    }
}
