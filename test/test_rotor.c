#include "unity.h"

#include "enigma/rotor.h"

#include <stdlib.h>

void test_enigma_rotor_generate_indices(void) {
    enigma_rotor_t rotor;
    const char*    alphabet = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
    int            ret      = enigma_rotor_generate_indices(&rotor, alphabet);
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(alphabet[i] - 'A', rotor.fwd_indices[i]);
    }
}

void test_enigma_rotor_generate_indices_WithInvalidArguments(void) {
    enigma_rotor_t rotor;
    const char*    alphabet = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_generate_indices(NULL, alphabet));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_generate_indices(&rotor, NULL));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_generate_indices(&rotor, "SHORT"));
}

void test_enigma_rotor_get_name(void) {
    enigma_rotor_t rotor;
    rotor.name = "RotorX";
    TEST_ASSERT_EQUAL_STRING("RotorX", enigma_rotor_get_name(&rotor));
}

void test_enigma_rotor_get_name_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_rotor_get_name(NULL));
}

void test_enigma_rotor_get_fwd_indices(void) {
    enigma_rotor_t rotor;
    for (int i = 0; i < 26; i++)
        rotor.fwd_indices[i] = i;
    const int* indices = enigma_rotor_get_fwd_indices(&rotor);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(i, indices[i]);
    }
}

void test_enigma_rotor_get_fwd_indices_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_rotor_get_fwd_indices(NULL));
}

void test_enigma_rotor_get_rev_indices(void) {
    enigma_rotor_t rotor;
    for (int i = 0; i < 26; i++)
        rotor.rev_indices[i] = 25 - i;
    const int* indices = enigma_rotor_get_rev_indices(&rotor);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(25 - i, indices[i]);
    }
}

void test_enigma_rotor_get_rev_indices_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_rotor_get_rev_indices(NULL));
}

void test_enigma_rotor_get_notches(void) {
    enigma_rotor_t rotor;
    rotor.notches[0]    = 5;
    rotor.notches[1]    = 17;
    rotor.notches_count = 2;
    const int* notches  = enigma_rotor_get_notches(&rotor);
    TEST_ASSERT_EQUAL_INT(5, notches[0]);
    TEST_ASSERT_EQUAL_INT(17, notches[1]);
}

void test_enigma_rotor_get_notches_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_rotor_get_notches(NULL));
}

void test_enigma_rotor_get_notches_count(void) {
    enigma_rotor_t rotor;
    rotor.notches_count = 2;
    TEST_ASSERT_EQUAL_INT(2, enigma_rotor_get_notches_count(&rotor));
}

void test_enigma_rotor_get_notches_count_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_get_notches_count(NULL));
}

void test_enigma_rotor_set_name(void) {
    enigma_rotor_t rotor;
    rotor.name = NULL;
    int ret    = enigma_rotor_set_name(&rotor, "RotorY");
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL(rotor.name);
    TEST_ASSERT_EQUAL_STRING("RotorY", rotor.name);
    free((void*) rotor.name);
}

void test_enigma_rotor_set_name_WithInvalidArguments(void) {
    enigma_rotor_t rotor;
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_name(NULL, "RotorZ"));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_name(&rotor, NULL));
}

void test_enigma_rotor_set_fwd_indices(void) {
    enigma_rotor_t rotor;
    int            indices[26];
    for (int i = 0; i < 26; i++)
        indices[i] = i;
    int ret = enigma_rotor_set_fwd_indices(&rotor, indices);
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(i, rotor.fwd_indices[i]);
    }
}

void test_enigma_rotor_set_fwd_indices_WithInvalidArguments(void) {
    enigma_rotor_t rotor;
    int            indices[26] = { 0 };
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_fwd_indices(NULL, indices));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_fwd_indices(&rotor, NULL));
}

void test_enigma_rotor_set_rev_indices(void) {
    enigma_rotor_t rotor;
    int            indices[26];
    for (int i = 0; i < 26; i++)
        indices[i] = 25 - i;
    int ret = enigma_rotor_set_rev_indices(&rotor, indices);
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(25 - i, rotor.rev_indices[i]);
    }
}

void test_enigma_rotor_set_rev_indices_WithInvalidArguments(void) {
    enigma_rotor_t rotor;
    int            indices[26] = { 0 };
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_rev_indices(NULL, indices));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_rev_indices(&rotor, NULL));
}

void test_enigma_rotor_set_notches(void) {
    enigma_rotor_t rotor;
    int            notches[2] = { 7, 13 };
    int            ret        = enigma_rotor_set_notches(&rotor, notches, 2);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(7, rotor.notches[0]);
    TEST_ASSERT_EQUAL_INT(13, rotor.notches[1]);
    TEST_ASSERT_EQUAL_INT(2, rotor.notches_count);
}

void test_enigma_rotor_set_notches_WithInvalidArguments(void) {
    enigma_rotor_t rotor;
    int            notches[2] = { 7, 13 };
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches(NULL, notches, 2));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches(&rotor, NULL, 2));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches(&rotor, notches, -1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches(&rotor, notches, 3));
}

void test_enigma_rotor_set_notches_count(void) {
    enigma_rotor_t rotor;
    int            ret = enigma_rotor_set_notches_count(&rotor, 2);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(2, rotor.notches_count);
}

void test_enigma_rotor_set_notches_count_WithInvalidArguments(void) {
    enigma_rotor_t rotor;
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches_count(NULL, 1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches_count(&rotor, -1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_rotor_set_notches_count(&rotor, 3));
}
