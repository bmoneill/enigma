#include "score.h"

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
    for (int i = 0; i < scoreList->scoreCount; i++) {
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

    qsort(scoreList->scores, scoreList->scoreCount, sizeof(enigma_score_t), score_compare);
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
