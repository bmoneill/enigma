#ifndef ENIGMA_NGRAM_H
#define ENIGMA_NGRAM_H

#include "crack.h"

typedef struct {
    char ngram[5];
    float score;
    int count;
} enigma_ngram_t;

typedef struct {
    enigma_ngram_t* ngrams;
    int count;
    int n;
} enigma_ngram_list_t;

void  enigma_crack_rotors_ngram         (enigma_crack_config_t*, enigma_ngram_list_t*);
void  enigma_crack_rotor_positions_ngram(enigma_crack_config_t*, enigma_ngram_list_t*);
void  enigma_crack_reflector_ngram      (enigma_crack_config_t*, enigma_ngram_list_t*);
void  enigma_crack_plugboard_ngram      (enigma_crack_config_t*, enigma_ngram_list_t*);

float enigma_bigram_score               (const char*,            const enigma_crack_config_t*, const float*);
float enigma_trigram_score              (const char*,            const enigma_crack_config_t*, const float*);
float enigma_quadram_score              (const char*,            const enigma_crack_config_t*, const float*);


#endif
