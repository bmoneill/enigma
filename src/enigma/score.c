#include "score.h"

#include "enigma/common.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

static int score_compare(const void* a, const void* b);

/**
 * @brief Print the scores in an enigma_score_list_t.
 *
 * @param scoreList Pointer to the enigma_score_list_t structure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_print(const enigma_score_list_t* scoreList) {
    if (!scoreList) {
        return 1;
    }
    char buf[64];
    for (int i = 0; i < scoreList->score_count; i++) {
        printf("%.6f ", scoreList->scores[i].score);
        if (scoreList->scores[i].flags & ENIGMA_FLAG_DICTIONARY_MATCH) {
            printf("D");
        } else {
            printf("-");
        }
        if (scoreList->scores[i].flags & ENIGMA_FLAG_FREQUENCY) {
            printf("F ");
        } else {
            printf("- ");
        }
        enigma_print_config(&scoreList->scores[i].enigma, buf);
        printf("%s\n", buf);
    }
    return 0;
}

/**
 * @brief Sort an array of enigma_score_t by score in descending order.
 *
 * @param scores The array of enigma_score_t to sort.
 *
 * @return 0 on success, non-zero on failure.
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_sort(enigma_score_list_t* scoreList) {
    if (!scoreList) {
        return 1;
    }

    qsort(scoreList->scores, scoreList->score_count, sizeof(enigma_score_t), score_compare);
    return 0;
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
 * @return The score field, or -1.0f if score is NULL
 */
EMSCRIPTEN_KEEPALIVE float enigma_score_get_score(enigma_score_t* score) {
    if (!score) {
        return -1.0f;
    }

    return score->score;
}

/**
 * @brief Get the flags field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @return The flags field, or -1 if score is NULL
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_get_flags(enigma_score_t* score) {
    if (!score) {
        return -1;
    }

    return score->flags;
}

/**
 * @brief Set the enigma field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @param enigma The enigma_t instance to set
 * @return 0 on success, 1 on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_enigma(enigma_score_t* score, enigma_t* enigma) {
    if (!score || !enigma) {
        return 1;
    }

    score->enigma = *enigma;
    return 0;
}

/**
 * @brief Set the score field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @param value The score value to set
 * @return 0 on success, 1 on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_score(enigma_score_t* score, float value) {
    if (!score) {
        return 1;
    }

    score->score = value;
    return 0;
}

/**
 * @brief Set the flags field in the given enigma_score_t
 *
 * @param score The enigma_score_t instance
 * @param flags The flags value to set
 * @return 0 on success, 1 on failure
 */
EMSCRIPTEN_KEEPALIVE int enigma_score_set_flags(enigma_score_t* score, int flags) {
    if (!score) {
        return 1;
    }

    score->flags = flags;
    return 0;
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
