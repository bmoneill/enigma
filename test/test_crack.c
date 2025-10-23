#include "enigma/crack.h"
#include "unity.h"
#include "enigma/crack.c"
#include "enigma/enigma.h"

#include <time.h>

enigma_crack_config_t cfg;

void setUp(void) {
    srand(time(NULL));
    memset(&cfg, 0, sizeof(enigma_crack_config_t));
}

void tearDown(void) { }

float mock_score_function(const enigma_crack_config_t* config, const char* plaintext) {
    return 0.0f;
}

void test_enigma_crack_plugboard(void) {
    // TODO Implement
}

void test_enigma_crack_plugboard_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(NULL, mock_score_function));
}

void test_enigma_crack_reflector(void) {
    // TODO Implement
}

void test_enigma_crack_reflector_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_reflector(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(NULL, mock_score_function));
}

void test_enigma_crack_rotor(void) {
    // TODO Implement
}

void test_enigma_crack_rotor_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor(NULL, 0, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor(&cfg, 0, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor(NULL, 0, mock_score_function));
}

void test_enigma_crack_rotors(void) {
    // TODO Implement
}

void test_enigma_crack_rotors_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotors(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotors(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotors(NULL, mock_score_function));
}

void test_enigma_crack_rotor_positions(void) {
    // TODO Implement
}

void test_enigma_crack_rotor_positions_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor_positions(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor_positions(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor_positions(NULL, mock_score_function));
}

void test_enigma_dict_match_WithMatchingPlaintext(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    cfg.dictionary = malloc(7 * sizeof(char*));
    cfg.dictSize = 7;

    cfg.dictionary[0] = "GOODBYE";
    cfg.dictionary[1] = "HELLO";
    cfg.dictionary[2] = "FOO";
    cfg.dictionary[3] = "BAZ";
    cfg.dictionary[4] = "BAR";
    cfg.dictionary[5] = "TEST";
    cfg.dictionary[6] = "WORLD";

    TEST_ASSERT_EQUAL_INT(1, enigma_dict_match(&cfg, plaintext));

    free(cfg.dictionary);
}

void test_enigma_dict_match_WithNonMatchingPlaintext(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";

    cfg.dictionary = malloc(4 * sizeof(char*));
    cfg.dictSize = 4;
    cfg.dictionary[0] = "FOO";
    cfg.dictionary[1] = "EARTH";
    cfg.dictionary[2] = "MARS";
    cfg.dictionary[3] = "SATURN";

    TEST_ASSERT_EQUAL_INT(0, enigma_dict_match(&cfg, plaintext));
}

void test_enigma_dict_match_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_dict_match(NULL, NULL));
    TEST_ASSERT_EQUAL_INT(-1, enigma_dict_match(&cfg, "HELLO"));
}

void test_enigma_find_potential_indices_WithMatchingPlaintext(void) {
    const char* plaintext = "HELLO";
    const char* ciphertext = "HLEOASFD";
    int* indices = malloc(sizeof(int) * strlen(ciphertext));

    int ret = enigma_find_potential_indices(ciphertext, plaintext, indices);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(2, indices[0]);
    TEST_ASSERT_EQUAL_INT(3, indices[1]);
    TEST_ASSERT_EQUAL_INT(-1, indices[2]);
    free(indices);
}

void test_enigma_find_potential_indices_WithNonMatchingPlaintext(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    const char* ciphertext = "GOODBYE";

    int* indices = malloc(sizeof(int) * strlen(ciphertext));

    int ret = enigma_find_potential_indices(ciphertext, plaintext, indices);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(-1, indices[0]);
    free(indices);
}

void test_enigma_freq(void) {
    const char* plaintext = "HELLO";
    float actual = enigma_freq(plaintext, strlen(plaintext));
    float expected = (2.0) / (5.0 * 4.0);
    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}

void test_enigma_freq_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_freq(NULL, 0));
}

void test_enigma_letter_freq_WithSufficientPlaintext(void) {
    const char* plaintext = "THEXQUICKXBROWNXFOXXJUMPSXOVERXTHEXLAZYXDOG";
    cfg.ciphertextLen = strlen(plaintext);

    for (int i = 0; i < 26; i++) {
        cfg.freqTargets[i] = 0.05;
    }
    cfg.freqOffset = 0.03;

    int actual = enigma_letter_freq(&cfg, plaintext);

    TEST_ASSERT_EQUAL_FLOAT(1, actual);
}

void test_enigma_letter_freq_WithInsufficientPlaintext(void) {
    const char* plaintext = "THEXQUICKXBROWNXFOXXJUMPSXOVERXTHEXLAZYXDOG";
    cfg.ciphertextLen = strlen(plaintext);
    for (int i = 0; i < 26; i++) {
        cfg.freqTargets[i] = 1.0;
    }
    cfg.freqOffset = 0.01;

    TEST_ASSERT_EQUAL_FLOAT(0, enigma_letter_freq(&cfg, plaintext));
}

void test_enigma_letter_freq_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_letter_freq(&cfg, NULL));
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_letter_freq(NULL, "HELLO"));
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_letter_freq(NULL, NULL));
}

void test_enigma_score_append_WithEmptyScoreList(void) {
    enigma_t enigma;
    const char* plaintext = "HELLOXWORLD";
    float score = 0.05;
    int maxScores = 10;

    enigma_init_default_config(&enigma);
    cfg.flags = 0;
    cfg.scores = malloc(sizeof(enigma_score_list_t));
    cfg.scores->maxScores = maxScores;
    cfg.scores->scoreCount = 0;
    cfg.scores->scores = malloc(sizeof(enigma_score_t) * cfg.scores->maxScores);

    int ret = enigma_score_append(&cfg, &enigma, plaintext, score);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, cfg.scores->scoreCount);
    TEST_ASSERT_EQUAL_INT(maxScores, cfg.scores->maxScores);
    TEST_ASSERT_EQUAL_FLOAT(score, cfg.scores->scores[0].score);
    TEST_ASSERT_EQUAL_INT_ARRAY(enigma.rotor_indices, cfg.scores->scores[0].enigma.rotor_indices, 4);

    free(cfg.scores->scores);
    free(cfg.scores);
}

void test_enigma_score_append_WithFullScoreList(void) {
    // TODO Implement
}

void test_enigma_score_append_WithPartialScoreList(void) {
    // TODO Implement
}

void test_enigma_score_append_WithNullArguments(void) {
    enigma_t enigma;
    const char* plaintext = "HELLOXWORLD";
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, NULL, NULL, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(&cfg, NULL, NULL, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(&cfg, &enigma, NULL, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(&cfg, NULL, plaintext, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, &enigma, plaintext, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, NULL, plaintext, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, &enigma, NULL, 0));
}

void test_enigma_score_flags_WithKnownPlaintextFlag_WithMatchingPlaintext(void) {
    cfg.flags = ENIGMA_FLAG_KNOWN_PLAINTEXT;
    cfg.plaintext = "WORLD";
    const char* plaintext = "HELLOXWORLD";
    int ret = enigma_score_flags(&cfg, plaintext);
    TEST_ASSERT_EQUAL_INT(ENIGMA_FLAG_KNOWN_PLAINTEXT, ret);
}

void test_enigma_score_flags_WithKnownPlaintextFlag_WithNonMatchingPlaintext(void) {
    cfg.flags = ENIGMA_FLAG_KNOWN_PLAINTEXT;
    cfg.plaintext = "WORLDS";
    const char* plaintext = "HELLOXWORLD";
    int ret = enigma_score_flags(&cfg, plaintext);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_score_flags_WithKnownPlaintextFlag_WithNullPlaintext(void) {
    cfg.flags = ENIGMA_FLAG_KNOWN_PLAINTEXT;
    cfg.plaintext = NULL;
    const char* plaintext = "HELLOXWORLD";

    int ret = enigma_score_flags(&cfg, plaintext);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_enigma_score_sort(void) {
    enigma_score_list_t scores;
    scores.maxScores = 10;
    scores.scores = malloc(sizeof(enigma_score_t) * scores.maxScores);
    scores.scoreCount = 10;
    for (int i = 0; i < scores.scoreCount; i++) {
        scores.scores[i].score = ((float) rand()) / ((float)rand());
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
