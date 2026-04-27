#include "enigma/brute.h"
#include "enigma/io.h"
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
    const char* dictStr   = "BAR\nBAZ\nFOO\nGOODBYE\nHELLO\nTEST\nWORLD";

    int         ret       = enigma_load_dict_s(&cfg, dictStr, strlen(dictStr));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, "Expected enigma_load_dict_s to succeed");

    ret = enigma_brute_score(&cfg, plaintext);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0, ret, "Expected dictionary to match");

    free(cfg.dictionary);
}

void test_enigma_brute_score_WithMatchingPlaintext_X_Separated(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    const char* dictStr   = "BAR\nBAZ\nFOO\nGOODBYE\nHELLO\nTEST\nWORLD";
    int         ret       = enigma_load_dict_s(&cfg, dictStr, strlen(dictStr));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, "Expected enigma_load_dict_s to succeed");
    cfg.flags |= ENIGMA_FLAG_X_SEPARATED;

    ret = enigma_brute_score(&cfg, plaintext);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0, ret, "Expected dictionary to match");
    free(cfg.dictionary);
}

void test_enigma_brute_score_WithNonMatchingPlaintext_X_Separated(void) {
    const char* dictStr   = "BAR\nBAZ\nEARTH\nFOO\nMARS\nSATURN\nTEST";
    cfg.ciphertext_length = strlen(helloWorld);
    cfg.flags |= ENIGMA_FLAG_X_SEPARATED;

    int ret = enigma_load_dict_s(&cfg, dictStr, strlen(dictStr));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, "Expected enigma_load_dict_s to succeed");

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0,
                                    enigma_brute_score(&cfg, helloWorld),
                                    "Expected dictionary not to match");
    free(cfg.dictionary);
}

void test_enigma_brute_score_WithNonMatchingPlaintext_Not_X_Separated(void) {
    const char* dictStr   = "EARTH\nFOO\nMARS\nSATURN";
    cfg.ciphertext_length = strlen(helloWorld);

    int ret               = enigma_load_dict_s(&cfg, dictStr, strlen(dictStr));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, "Expected enigma_load_dict_s to succeed");

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0,
                                    enigma_brute_score(&cfg, helloWorld),
                                    "Expected dictionary not to match");
    free(cfg.dictionary);
}
