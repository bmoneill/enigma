#include "unity.h"
#include "enigma/crack.h"
#include "enigma/ioc.h"

void setUp(void) { }

void tearDown(void) { }

void test_enigma_ioc_score(void) {
    enigma_crack_config_t cfg;
    const char* text = "HELLOWORLD";
    cfg.ciphertextLen = strlen(text);

    float score = enigma_ioc_score(&cfg, text);
    TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, score);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_enigma_ioc_score);

    return UNITY_END();
}
