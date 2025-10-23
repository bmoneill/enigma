#include "unity.h"
#include "enigma/crack.h"
#include "enigma/ioc.h"

#include <string.h>

void test_enigma_ioc_score(void) {
    enigma_crack_config_t cfg;
    const char* text = "HELLOWORLD";
    cfg.ciphertextLen = strlen(text);

    float score = enigma_ioc_score(&cfg, text);
    TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, score);
}
