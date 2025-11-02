#include "enigma/reflector.h"
#include "unity.h"

#include <stdlib.h>

#include "enigma/reflector.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_enigma_reflector_generate_indices(void) {
    enigma_reflector_t reflector;
    const char*        alphabet = "EJMZALYXVBWFCRQUONTSPIKHGD";
    int                ret      = enigma_reflector_generate_indices(&reflector, alphabet);
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(alphabet[i] - 'A', reflector.indices[i]);
    }
}

void test_enigma_reflector_generate_indices_WithInvalidArguments(void) {
    enigma_reflector_t reflector;
    const char*        alphabet = "EJMZALYXVBWFCRQUONTSPIKHGD";
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_generate_indices(NULL, alphabet));
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_generate_indices(&reflector, NULL));
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_generate_indices(&reflector, "SHORT"));
}

void test_enigma_reflector_get_name(void) {
    enigma_reflector_t reflector;
    reflector.name   = "TestReflector";
    const char* name = enigma_reflector_get_name(&reflector);
    TEST_ASSERT_EQUAL_STRING("TestReflector", name);
}

void test_enigma_reflector_get_name_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_reflector_get_name(NULL));
}

void test_enigma_reflector_get_indices(void) {
    enigma_reflector_t reflector;
    for (int i = 0; i < 26; i++)
        reflector.indices[i] = i;
    const int* indices = enigma_reflector_get_indices(&reflector);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(i, indices[i]);
    }
}

void test_enigma_reflector_get_indices_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_reflector_get_indices(NULL));
}

void test_enigma_reflector_set_name(void) {
    enigma_reflector_t reflector;
    reflector.name = NULL;
    int ret        = enigma_reflector_set_name(&reflector, "MyReflector");
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL(reflector.name);
    TEST_ASSERT_EQUAL_STRING("MyReflector", reflector.name);
    free((void*) reflector.name);
}

void test_enigma_reflector_set_name_WithInvalidArguments(void) {
    enigma_reflector_t reflector;
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_set_name(NULL, "MyReflector"));
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_set_name(&reflector, NULL));
}

void test_enigma_reflector_set_indices(void) {
    enigma_reflector_t reflector;
    int                indices[26];
    for (int i = 0; i < 26; i++)
        indices[i] = 25 - i;
    int ret = enigma_reflector_set_indices(&reflector, indices);
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_INT(25 - i, reflector.indices[i]);
    }
}

void test_enigma_reflector_set_indices_WithInvalidArguments(void) {
    enigma_reflector_t reflector;
    int                indices[26] = { 0 };
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_set_indices(NULL, indices));
    TEST_ASSERT_EQUAL_INT(-1, enigma_reflector_set_indices(&reflector, NULL));
}
