#include "enigma/score.c"

#include "unity.h"

// TODO enigma_score_list_t getter/setter tests
// TODO enigma_score_t getter/setter tests

void test_enigma_score_sort(void) {
    enigma_score_list_t scores;
    scores.maxScores  = 10;
    scores.scores     = malloc(sizeof(enigma_score_t) * scores.maxScores);
    scores.scoreCount = 10;
    for (int i = 0; i < scores.scoreCount; i++) {
        scores.scores[i].score = ((float) rand()) / ((float) rand());
    }

    int ret = enigma_score_sort(&scores);

    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 1; i < scores.scoreCount; i++) {
        TEST_ASSERT_TRUE(scores.scores[i - 1].score >= scores.scores[i].score);
    }

    free(scores.scores);
}

void test_enigma_score_sort_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_INT(1, enigma_score_sort(NULL));
}

void test_score_compare(void) {
    enigma_score_t score1;
    enigma_score_t score2;
    score1.score = 0.5;
    score2.score = 0.6;

    TEST_ASSERT_EQUAL_INT(1, score_compare(&score1, &score2));
    TEST_ASSERT_EQUAL_INT(-1, score_compare(&score2, &score1));
    TEST_ASSERT_EQUAL_INT(0, score_compare(&score1, &score1));
}
