#include "enigma/score.c"

#include "unity.h"

void test_enigma_score_sort(void) {
    enigma_score_list_t scores;
    scores.max_scores  = 10;
    scores.scores      = malloc(sizeof(enigma_score_t) * scores.max_scores);
    scores.score_count = 10;
    for (int i = 0; i < scores.score_count; i++) {
        scores.scores[i].score = ((float) rand()) / ((float) rand());
    }

    int ret = enigma_score_list_sort(&scores);

    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 1; i < scores.score_count; i++) {
        TEST_ASSERT_TRUE(scores.scores[i - 1].score >= scores.scores[i].score);
    }

    free(scores.scores);
}

void test_enigma_score_sort_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_sort(NULL));
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

void test_enigma_score_get_enigma(void) {
    enigma_score_t score;
    enigma_t* result = enigma_score_get_enigma(&score);
    TEST_ASSERT_EQUAL_PTR(&score.enigma, result);
}

void test_enigma_score_get_enigma_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_score_get_enigma(NULL));
}

void test_enigma_score_get_score(void) {
    enigma_score_t score;
    score.score = 42.0f;
    TEST_ASSERT_EQUAL_FLOAT(42.0f, enigma_score_get_score(&score));
}

void test_enigma_score_get_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_score_get_score(NULL));
}

void test_enigma_score_get_flags(void) {
    enigma_score_t score;
    score.flags = 123;
    TEST_ASSERT_EQUAL_INT(123, enigma_score_get_flags(&score));
}

void test_enigma_score_get_flags_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_get_flags(NULL));
}

void test_enigma_score_set_enigma(void) {
    enigma_score_t score;
    enigma_t enigma;
    int ret = enigma_score_set_enigma(&score, &enigma);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_score_set_enigma_WithInvalidArguments(void) {
    enigma_t enigma;
    enigma_score_t score;
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_enigma(NULL, &enigma));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_enigma(&score, NULL));
}

void test_enigma_score_set_score(void) {
    enigma_score_t score;
    int ret = enigma_score_set_score(&score, 3.14f);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_FLOAT(3.14f, score.score);
}

void test_enigma_score_set_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_score(NULL, 1.23f));
}

void test_enigma_score_set_flags(void) {
    enigma_score_t score;
    int ret = enigma_score_set_flags(&score, 7);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(7, score.flags);
}

void test_enigma_score_set_flags_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_set_flags(NULL, 5));
}

void test_enigma_score_list_get_scores(void) {
    enigma_score_list_t list;
    enigma_score_t arr[2];
    list.scores = arr;
    TEST_ASSERT_EQUAL_PTR(arr, enigma_score_list_get_scores(&list));
}

void test_enigma_score_list_get_scores_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_score_list_get_scores(NULL));
}

void test_enigma_score_list_get_score(void) {
    enigma_score_list_t list;
    enigma_score_t arr[2];
    list.scores = arr;
    list.score_count = 2;
    enigma_score_t* result = enigma_score_list_get_score(&list, 1);
    TEST_ASSERT_EQUAL_PTR(&arr[1], result);
}

void test_enigma_score_list_get_score_WithInvalidArguments(void) {
    enigma_score_list_t list;
    enigma_score_t arr[2];
    list.scores = arr;
    list.score_count = 2;
    TEST_ASSERT_NULL(enigma_score_list_get_score(NULL, 0));
    TEST_ASSERT_NULL(enigma_score_list_get_score(&list, -1));
    TEST_ASSERT_NULL(enigma_score_list_get_score(&list, 2));
}

void test_enigma_score_list_get_score_count(void) {
    enigma_score_list_t list;
    list.score_count = 5;
    TEST_ASSERT_EQUAL_INT(5, enigma_score_list_get_score_count(&list, 0));
}

void test_enigma_score_list_get_score_count_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_get_score_count(NULL, 0));
}

void test_enigma_score_list_get_max_scores(void) {
    enigma_score_list_t list;
    list.max_scores = 10;
    TEST_ASSERT_EQUAL_INT(10, enigma_score_list_get_max_scores(&list));
}

void test_enigma_score_list_get_max_scores_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_get_max_scores(NULL));
}

void test_enigma_score_list_set_score(void) {
    enigma_score_list_t list;
    enigma_score_t arr[2];
    enigma_score_t score;
    list.scores = arr;
    list.score_count = 2;
    int ret = enigma_score_list_set_score(&list, 1, &score);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_PTR(&arr[1], &list.scores[1]);
}

void test_enigma_score_list_set_score_WithInvalidArguments(void) {
    enigma_score_list_t list;
    enigma_score_t arr[2];
    enigma_score_t score;
    list.scores = arr;
    list.score_count = 2;
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(NULL, 1, &score));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(&list, 1, NULL));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(&list, -1, &score));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score(&list, 2, &score));
}

void test_enigma_score_list_set_score_count(void) {
    enigma_score_list_t list;
    int ret = enigma_score_list_set_score_count(&list, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(3, list.score_count);
}

void test_enigma_score_list_set_score_count_WithInvalidArguments(void) {
    enigma_score_list_t list;
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score_count(NULL, 1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_score_count(&list, -1));
}

void test_enigma_score_list_set_max_scores(void) {
    enigma_score_list_t list;
    int ret = enigma_score_list_set_max_scores(&list, 7);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(7, list.max_scores);
}

void test_enigma_score_list_set_max_scores_WithInvalidArguments(void) {
    enigma_score_list_t list;
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_max_scores(NULL, 1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_score_list_set_max_scores(&list, -1));
}
