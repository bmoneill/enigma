#include "unity.h"
#include "enigma/crack.c"

void setUp(void) { }
void tearDown(void) { }

void test_enigma_crack_plugboard(void) {
    // TODO Implement
}

void test_enigma_crack_reflector(void) {
    // TODO Implement
}

void test_enigma_crack_rotor(void) {
    // TODO Implement
}

void test_enigma_crack_rotors(void) {
    // TODO Implement
}

void test_enigma_crack_rotor_positions(void) {
    // TODO Implement
}

void test_enigma_dict_match(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    enigma_crack_config_t cfg;
    cfg.dictionary = malloc(4 * sizeof(char*));
    cfg.dictSize = 7;

    cfg.dictionary[0] = "GOODBYE";
    cfg.dictionary[1] = "HELLO";
    cfg.dictionary[2] = "FOO";
    cfg.dictionary[3] = "BAZ";
    cfg.dictionary[4] = "BAR";
    cfg.dictionary[5] = "TEST";
    cfg.dictionary[6] = "WORLD";
    TEST_ASSERT_EQUAL_INT(1, enigma_dict_match(&cfg, plaintext));

    cfg.dictionary[1] = "FOOBAR";
    cfg.dictionary[2] = "MOON";
    cfg.dictionary[6] = "MARS";
    TEST_ASSERT_EQUAL_INT(0, enigma_dict_match(&cfg, plaintext));
    free(cfg.dictionary);
}

void test_enigma_find_potential_indices(void) {
    const char* plaintext = "HELLO";
    const char* ciphertext = "HLEOASFD";

    int* indices = malloc(sizeof(int) * strlen(ciphertext));

    enigma_find_potential_indices(ciphertext, plaintext, indices);

    TEST_ASSERT_EQUAL_INT(2, indices[0]);
    TEST_ASSERT_EQUAL_INT(3, indices[1]);
    TEST_ASSERT_EQUAL_INT(-1, indices[2]);
    free(indices);
}

void test_enigma_freq(void) {
    const char* plaintext = "HELLO";

    float actual = enigma_freq(plaintext, strlen(plaintext));

    float expected = (2.0) / (5.0 * 4.0);
    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}

void test_enigma_letter_freq(void) {
    // TODO Implement
}

void test_enigma_score_append(void) {
    // TODO Implement
}

void test_enigma_score_flags(void) {
    // TODO Implement
}

void test_enigma_score_sort(void) {
    // TODO Implement
}

void test_score_compare(void) {
    // TODO Implement
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_enigma_find_potential_indices);
    RUN_TEST(test_enigma_freq);
    return UNITY_END();
}
