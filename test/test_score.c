#include "enigma/score.c"

#include "unity.h"

#include <string.h>

const char*     success = "Expected success";
const char*     failure = "Expected failure";

EnigmaScoreList scores;

void            setUp(void) { memset(&scores, 0, sizeof(EnigmaScoreList)); }

void            test_enigma_score_sort(void) {
    scores.max_scores  = 10;
    scores.scores      = malloc(sizeof(EnigmaScore) * scores.max_scores);
    scores.score_count = 10;
    for (int i = 0; i < scores.score_count; i++) {
        scores.scores[i].score = ((float) rand()) / ((float) rand());
    }

    int ret = enigma_score_list_sort(&scores);

    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, success);
    for (int i = 1; i < scores.score_count; i++) {
        TEST_ASSERT_TRUE_MESSAGE(scores.scores[i - 1].score >= scores.scores[i].score,
                                 "Expected scores to be sorted");
    }

    free(scores.scores);
}

void test_enigma_score_sort_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, enigma_score_list_sort(NULL), failure);
}

void test_enigma_score_compare(void) {
    EnigmaScore score1;
    EnigmaScore score2;
    score1.score = 0.5;
    score2.score = 0.6;

    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
                                  enigma_score_compare(&score1, &score2),
                                  "Expected score1 to be greater than score2");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
                                  enigma_score_compare(&score2, &score1),
                                  "Expected score2 to be greater than score1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  enigma_score_compare(&score1, &score1),
                                  "Expected score1 to be equal to score1");
}

// --- EnigmaScore getter/setter tests ---

void test_enigma_score_get_enigma(void) {
    EnigmaScore score;
    Enigma*     result = enigma_score_get_enigma(&score);
    TEST_ASSERT_EQUAL_PTR(&score.enigma, result);
}

void test_enigma_score_get_enigma_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_score_get_enigma(NULL));
}

void test_enigma_score_get_score(void) {
    EnigmaScore score;
    score.score = 42.0f;
    TEST_ASSERT_EQUAL_FLOAT(42.0f, enigma_score_get_score(&score));
}

void test_enigma_score_get_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_score_get_score(NULL));
}

void test_enigma_score_get_flags(void) {
    EnigmaScore score;
    score.flags = 123;
    TEST_ASSERT_EQUAL_INT(123, enigma_score_get_flags(&score));
}

void test_enigma_score_get_flags_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_get_flags(NULL));
}

void test_enigma_score_set_enigma(void) {
    EnigmaScore score;
    Enigma      enigma;
    int         ret = enigma_score_set_enigma(&score, &enigma);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_score_set_enigma_WithInvalidArguments(void) {
    Enigma      enigma;
    EnigmaScore score;
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_enigma(NULL, &enigma));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_enigma(&score, NULL));
}

void test_enigma_score_set_score(void) {
    EnigmaScore score;
    int         ret = enigma_score_set_score(&score, 3.14f);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_FLOAT(3.14f, score.score);
}

void test_enigma_score_set_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_score(NULL, 1.23f));
}

void test_enigma_score_set_flags(void) {
    EnigmaScore score;
    int         ret = enigma_score_set_flags(&score, 7);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(7, score.flags);
}

void test_enigma_score_set_flags_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_flags(NULL, 5));
}

// --- enigma_score_list_t getter/setter tests ---t

void test_enigma_score_list_get_scores(void) {
    EnigmaScore arr[2];
    scores.scores = arr;
    TEST_ASSERT_EQUAL_PTR(arr, enigma_score_list_get_scores(&scores));
}

void test_enigma_score_list_get_scores_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_score_list_get_scores(NULL));
}

void test_enigma_score_list_get_score(void) {
    EnigmaScore arr[2];
    scores.scores       = arr;
    scores.score_count  = 2;
    EnigmaScore* result = enigma_score_list_get_score(&scores, 1);
    TEST_ASSERT_EQUAL_PTR(&arr[1], result);
}

void test_enigma_score_list_get_score_WithInvalidArguments(void) {
    EnigmaScore arr[2];
    scores.scores      = arr;
    scores.score_count = 2;
    TEST_ASSERT_NULL(enigma_score_list_get_score(NULL, 0));
    TEST_ASSERT_NULL(enigma_score_list_get_score(&scores, -1));
    TEST_ASSERT_NULL(enigma_score_list_get_score(&scores, 2));
}

void test_enigma_score_list_get_score_count(void) {
    scores.score_count = 5;
    TEST_ASSERT_EQUAL_INT(5, enigma_score_list_get_score_count(&scores));
}

void test_enigma_score_list_get_score_count_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_get_score_count(NULL));
}

void test_enigma_score_list_get_max_scores(void) {
    scores.max_scores = 10;
    TEST_ASSERT_EQUAL_INT(10, enigma_score_list_get_max_scores(&scores));
}

void test_enigma_score_list_get_max_scores_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_get_max_scores(NULL));
}

void test_enigma_score_list_set_score(void) {
    EnigmaScore arr[2];
    EnigmaScore score;
    scores.scores      = arr;
    scores.score_count = 2;
    int ret            = enigma_score_list_set_score(&scores, 1, &score);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_PTR(&arr[1], &scores.scores[1]);
}

void test_enigma_score_list_set_score_WithInvalidArguments(void) {
    EnigmaScore arr[2];
    EnigmaScore score;
    scores.scores      = arr;
    scores.score_count = 2;
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(NULL, 1, &score));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(&scores, 1, NULL));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(&scores, -1, &score));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(&scores, 2, &score));
}

void test_enigma_score_list_set_score_count(void) {
    int ret = enigma_score_list_set_score_count(&scores, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(3, scores.score_count);
}

void test_enigma_score_list_set_score_count_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score_count(NULL, 1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score_count(&scores, -1));
}

void test_enigma_score_list_set_max_scores(void) {
    int ret = enigma_score_list_set_max_scores(&scores, 7);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(7, scores.max_scores);
}

void test_enigma_score_list_set_max_scores_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_max_scores(NULL, 1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_max_scores(&scores, -1));
}
