#include "enigma/crack.h"
#include "enigma/ngram.h"
#include "unity.h"

#include <stdlib.h>
#include <string.h>

#define I(s) (s - 'A')

enigma_crack_t cfg;
const char*    plaintext = "EVERXTRIEDXEVERXFAILEDXNOXMATTERXTRYXAGAINXFAILXAGAINXFAILXBETTER";

void           setUp(void) {
    memset(&cfg, 0, sizeof(enigma_crack_t));
    cfg.ciphertext_length = strlen(plaintext);
}

void loadNgrams(int n) {
    int len = strlen(plaintext);
    for (int i = 0; i < len - 7; i++) {
        switch (n) {
        case 2:
            cfg.ngrams[ENIGMA_BIIDX(I(plaintext[i]), I(plaintext[i + 1]))] += 1.0 / len;
            break;
        case 3:
            cfg.ngrams[ENIGMA_TRIIDX(I(plaintext[i]), I(plaintext[i + 1]), I(plaintext[i + 2]))]
                += 1.0 / len;
            break;
        case 4:
            cfg.ngrams[ENIGMA_QUADIDX(I(plaintext[i]),
                                      I(plaintext[i + 1]),
                                      I(plaintext[i + 2]),
                                      I(plaintext[i + 3]))]
                += 1.0 / len;
            break;
        }
    }
}

void test_enigma_bigram_score(void) {
    cfg.n      = 2;
    cfg.ngrams = calloc((26 << 5) | 26, sizeof(float));

    loadNgrams(2);

    float score = enigma_bigram_score(&cfg, plaintext);
    TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(0.0f,
                                           score,
                                           "Expected bigram score to be greater than 0");

    free(cfg.ngrams);
}

void test_enigma_trigram_score(void) {
    cfg.ciphertext_length = strlen(plaintext);
    cfg.n                 = 3;
    cfg.ngrams            = calloc((26 << 10) | (26 << 5) | 26, sizeof(float));

    loadNgrams(3);

    float score = enigma_trigram_score(&cfg, plaintext);
    TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(0.0f,
                                           score,
                                           "Expected trigram score to be greater than 0");

    free(cfg.ngrams);
}

void test_enigma_quadgram_score(void) {
    cfg.ciphertext_length = strlen(plaintext);
    cfg.n                 = 4;
    cfg.ngrams            = calloc((26 << 15) | (26 << 10) | (26 << 5) | 26, sizeof(float));

    loadNgrams(4);

    float score = enigma_quadgram_score(&cfg, plaintext);
    TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(0.0f,
                                           score,
                                           "Expected quadgram score to be greater than 0");

    free(cfg.ngrams);
}
