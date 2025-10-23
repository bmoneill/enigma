#include "unity.h"
#include "enigma/ngram.h"

#include <string.h>

void setUp(void) { }

void tearDown(void) { }

void test_enigma_bigram_score(void) {
    enigma_crack_config_t cfg;
    const char* text = "ABCD";
    cfg.ciphertextLen = strlen(text);

    // TODO Prepare n-grams

    //float score = enigma_bigram_score(&cfg, text);
    //TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, score);
}

void test_enigma_trigram_score(void) {
    enigma_crack_config_t cfg;
    const char* text = "HELLOWORLD";
    cfg.ciphertextLen = strlen(text);

    // TODO Prepare n-grams

    //float score = enigma_trigram_score(&cfg, text);
    //TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, score);
}

void test_enigma_quadgram_score(void) {
    enigma_crack_config_t cfg;
    const char* text = "HELLOWORLD";
    cfg.ciphertextLen = strlen(text);

    // TODO Prepare n-grams

    //float score = enigma_quadgram_score(&cfg, text);
    //TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, score);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_enigma_bigram_score);
    RUN_TEST(test_enigma_trigram_score);
    RUN_TEST(test_enigma_quadgram_score);
    return UNITY_END();
}
