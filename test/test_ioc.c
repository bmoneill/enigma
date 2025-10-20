#include "unity.h"
#include "enigma/crack.h"
#include "enigma/ioc.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) { }

void tearDown(void) { }

void test_enigma_ioc_score(void) {
    enigma_crack_config_t cfg;
    const char* text = "HELLOWORLD";

    float score = enigma_ioc_score(text, &cfg);
    TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, score);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_enigma_ioc_score);

    return UNITY_END();
}
