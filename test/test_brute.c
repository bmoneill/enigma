#include "enigma/brute.h"
#include "unity.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

EnigmaCrackParams cfg;
const char*       helloWorld = "HELLOXWORLD";

void              setUp(void) {
    srand(time(NULL));
    memset(&cfg, 0, sizeof(EnigmaCrackParams));
    enigma_init_default_config(&cfg.enigma);
}

void tearDown(void) {}

void test_enigma_brute_score_WithMatchingPlaintext_Not_X_Separated(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    cfg.ciphertext_length = strlen(plaintext);
    cfg.dictionary        = malloc(7 * sizeof(char*));
    cfg.dictionary_length = 7;

    cfg.dictionary[0]     = "BAR";
    cfg.dictionary[1]     = "BAZ";
    cfg.dictionary[2]     = "FOO";
    cfg.dictionary[3]     = "GOODBYE";
    cfg.dictionary[4]     = "HELLO";
    cfg.dictionary[5]     = "TEST";
    cfg.dictionary[6]     = "WORLD";

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0,
                                    enigma_brute_score(&cfg, plaintext),
                                    "Expected dictionary to match");

    free(cfg.dictionary);
}

void test_enigma_brute_score_WithMatchingPlaintext_X_Separated(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    cfg.ciphertext_length = strlen(plaintext);
    cfg.dictionary        = malloc(7 * sizeof(char*));
    cfg.dictionary_length = 7;
    cfg.flags |= ENIGMA_FLAG_X_SEPARATED;

    cfg.dictionary[0] = "BAR";
    cfg.dictionary[1] = "BAZ";
    cfg.dictionary[2] = "FOO";
    cfg.dictionary[3] = "GOODBYE";
    cfg.dictionary[4] = "HELLO";
    cfg.dictionary[5] = "TEST";
    cfg.dictionary[6] = "WORLD";

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0,
                                    enigma_brute_score(&cfg, plaintext),
                                    "Expected dictionary to match");
    free(cfg.dictionary);
}

void test_enigma_brute_score_WithNonMatchingPlaintext_X_Separated(void) {
    cfg.dictionary        = malloc(7 * sizeof(char*));
    cfg.dictionary_length = 7;
    cfg.ciphertext_length = strlen(helloWorld);
    cfg.flags |= ENIGMA_FLAG_X_SEPARATED;
    cfg.dictionary[0] = "BAR";
    cfg.dictionary[1] = "BAZ";
    cfg.dictionary[2] = "EARTH";
    cfg.dictionary[3] = "FOO";
    cfg.dictionary[4] = "MARS";
    cfg.dictionary[5] = "SATURN";
    cfg.dictionary[6] = "TEST";

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0,
                                    enigma_brute_score(&cfg, helloWorld),
                                    "Expected dictionary not to match");
    free(cfg.dictionary);
}

void test_enigma_brute_score_WithNonMatchingPlaintext_Not_X_Separated(void) {
    cfg.dictionary        = malloc(4 * sizeof(char*));
    cfg.ciphertext_length = strlen(helloWorld);
    cfg.dictionary_length = 4;
    cfg.dictionary[0]     = "EARTH";
    cfg.dictionary[1]     = "FOO";
    cfg.dictionary[2]     = "MARS";
    cfg.dictionary[3]     = "SATURN";

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0,
                                    enigma_brute_score(&cfg, helloWorld),
                                    "Expected dictionary not to match");
    free(cfg.dictionary);
}
