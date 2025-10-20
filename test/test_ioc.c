#include "unity.h"
#include "enigma/ioc.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) { }

void tearDown(void) { }

void test_enigma_ioc_score(void) {
    enigma_crack_config_t* cfg = malloc(sizeof(enigma_crack_config_t));
    const char* text = "HELLOTHERE";
    cfg->ciphertextLen = strlen(text);

    float score = enigma_ioc_score(text, cfg);
    TEST_ASSERT_GREATER_THAN(0, score);
    free(cfg);
}

int main(void) {
    UNITY_BEGIN();

    test_enigma_ioc_score();

    return UNITY_END();
}
