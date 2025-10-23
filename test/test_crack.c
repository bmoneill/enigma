#include "unity.h"
#include "enigma/crack.c"
#include "enigma/enigma.h"

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
    enigma_crack_config_t cfg;
    const char* plaintext = "HELLOXWORLD";

    cfg.freqTargets['H' - 'A'] = 0.05;
    cfg.freqTargets['E' - 'A'] = 0.05;
    cfg.freqTargets['L' - 'A'] = 0.05;
    cfg.freqTargets['O' - 'A'] = 0.05;
    cfg.freqTargets['W' - 'A'] = 0.05;
    cfg.freqTargets['R' - 'A'] = 0.05;
    cfg.freqTargets['D' - 'A'] = 0.05;

    int actual = enigma_letter_freq(&cfg, plaintext);

    TEST_ASSERT_EQUAL_FLOAT(0, actual);
}

void test_enigma_score_append(void) {
    enigma_t enigma;
    enigma_crack_config_t cfg;
    const char* plaintext = "HELLOXWORLD";
    float score = 0.05;
    int maxScores = 10;

    cfg.flags = 0;
    cfg.scores = malloc(sizeof(enigma_score_list_t));
    cfg.scores->maxScores = maxScores;
    cfg.scores->scoreCount = 0;
    cfg.scores->scores = malloc(sizeof(enigma_score_t) * cfg.scores->maxScores);

    enigma_init_default_config(&enigma);

    int result = enigma_score_append(&cfg, &enigma, plaintext, score);
    TEST_ASSERT_EQUAL_INT(0, result);

    TEST_ASSERT_EQUAL_INT(1, cfg.scores->scoreCount);
    TEST_ASSERT_EQUAL_INT(maxScores, cfg.scores->maxScores);
    TEST_ASSERT_EQUAL_FLOAT(score, cfg.scores->scores[0].score);
    TEST_ASSERT_EQUAL_INT_ARRAY(enigma.rotor_indices, cfg.scores->scores[0].enigma.rotor_indices, 4);

    free(cfg.scores->scores);
    free(cfg.scores);
}

void test_enigma_score_flags(void) {
    const char* plaintext = "HELLOXWORLD";
    enigma_t enigma;

    enigma_crack_config_t cfg;
    cfg.flags = 0;
    cfg.scores = malloc(sizeof(enigma_score_list_t));
    cfg.scores->maxScores = 10;
    cfg.scores->scoreCount = 0;
    cfg.scores->scores = malloc(sizeof(enigma_score_t) * cfg.scores->maxScores);

    enigma_init_default_config(&enigma);

    int result = enigma_score_flags(&cfg, plaintext);

    TEST_ASSERT_EQUAL_INT(0, result);

    // TODO Test ENIGMA_FLAG_DICTIONARY_MATCH
    // TODO Test ENIGMA_FLAG_FREQUENCY
    // TODO Test ENIGMA_FLAG_KNOWN_PLAINTEXT

    free(cfg.scores->scores);
    free(cfg.scores);
}

void test_enigma_score_sort(void) {
    // TODO Implement
}

void test_score_compare(void) {
    // TODO Implement
}

int main(void) {
    UNITY_BEGIN();
}
