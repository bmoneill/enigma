#ifndef ENIGMA_NGRAM_H
#define ENIGMA_NGRAM_H

float enigma_bigram_score(const char *text, int textLen, const float *bigrams);
float enigma_trigram_score(const char *text, int textLen, const float *trigrams);
float enigma_quadram_score(const char *text, int textLen, const float *quadgrams);

#endif
