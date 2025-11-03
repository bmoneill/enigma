#include "enigma/crack.h"
#include "enigma/ioc.h"
#include "unity.h"

#include <string.h>

void test_enigma_ioc_score(void) {
    enigma_crack_t cfg;
    const char*    text   = "HELLOWORLD";
    cfg.ciphertext_length = strlen(text);
    float score           = enigma_ioc_score(&cfg, text);
    TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(0.0f, score, "Expected IOC score to be greater than 0");
}
